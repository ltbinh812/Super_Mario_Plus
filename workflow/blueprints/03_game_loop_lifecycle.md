# Blueprint 3: Game Loop Lifecycle & File Responsibilities

Tài liệu này mô tả chi tiết 1 frame của Game Loop, luồng thực thi (flow of execution) đi qua những hàm nào và nằm ở các file nào. Kiến trúc chia rõ 4 bước: **Input → Process → Update → Render**.

### Vai trò của các Core Manager
* **`BaseLevelState` (Orchestrator)**: Quản lý toàn bộ vòng đời của một Level. Nó chứa tất cả Entities, Items, Map, Camera, và Hệ thống Combat. Nó điều phối 4 luồng vòng lặp chính của trò chơi (Input, Process, Update, Render) và quản lý việc chuyển cảnh (Load map mới).
* **`TileMap` (Environment Provider)**: Quản lý dữ liệu tĩnh của bản đồ tải từ file LDtk (JSON). Nó lưu trữ cấu trúc địa hình, cung cấp hàm `GetCollidingTiles()` cho vật lý, gom nhóm kết xuất tĩnh vào một `RenderTexture2D` (Canvas) để tối ưu, và chứa dữ liệu thô để sinh quái/vật phẩm (`entityData_`).

---

## The Core Loop (1 Frame)

Đây là hàm `Game::Run()` nằm trong `src/core/Game.cpp`.

```mermaid
flowchart TD
    StartFrame([Start Frame: Game::Run loop]) --> HandleInput
    
    subgraph S1 [1. HANDLE INPUT (Variable Timestep)]
        direction TB
        HandleInput(BaseLevelState::HandleInput) --> InputHandler
        InputHandler(InputHandler::handleInput) -.->|Returns vector of Commands| LoopCommands
        LoopCommands(Execute Commands) --> MoveCmd(e.g. MoveRightCommand::Execute)
        MoveCmd --> PlayerFunc(Player::onMoveRight)
        PlayerFunc --> StateFunc(PlayerState::onMoveRight)
    end
    
    S1 --> Process
    
    subgraph S2 [2. PROCESS (Variable Timestep)]
        direction TB
        Process(BaseLevelState::Process) --> DeathCheck(Check Health <= 0)
        DeathCheck --> DropCmd(CommandQueue::peekAndConsumeByCategory)
        DropCmd --> ApplyEffect(Entity::addEffect)
        ApplyEffect --> RemoveDead(activeEntities.erase)
        RemoveDead --> Trans(Check Map Transition)
        Trans -.->|If switching map| LoadMap(TileMap::LoadLDtkMap)
        Trans -.->|Else| CamUpdate(MapCamera::Update)
    end
    
    Process --> Update
    
    subgraph S3 [3. UPDATE (Fixed Timestep - 60Hz)]
        direction TB
        Update(BaseLevelState::Update) --> SolidGen(Generate dynamic solids from Items)
        SolidGen --> PhysicsP1(Player1::updatePhysicsWithMap)
        PhysicsP1 --> EntityUp(Entity::updatePhysicsWithMap)
        EntityUp --> ResolveCol(TileMap::GetCollidingTiles)
        ResolveCol --> Trigger(Entity::handleTriggers)
        Trigger --> P1State(Player1::updateStateFromPhysics)
        P1State --> P1Up(Player1::update)
        P1Up --> ItemUp(BaseItem::update)
        ItemUp --> ItemCol(Check Item-Player Collision)
        ItemCol --> Interact(BaseItem::onInteract)
        Interact --> PopQueue(CommandQueue::popAll)
        PopQueue -.->|SpawnCommand| Fac(EntityFactory/ItemFactory::create)
        Fac --> CombatUp(CombatSystem::update)
    end
    
    Update --> Render
    
    subgraph S4 [4. RENDER (Variable Timestep)]
        direction TB
        Render(BaseLevelState::Render) --> CamBegin(MapCamera::BeginMode)
        CamBegin --> DrawMap(TileMap::Draw)
        DrawMap --> DrawItems(BaseItem::render)
        DrawItems --> DrawEntities(Entity::render)
        DrawEntities --> DrawP1(Player::render)
        DrawP1 --> AnimFrame(Animation::getCurrentFrame)
        AnimFrame --> CamEnd(MapCamera::EndMode)
    end
    
    Render --> EndFrame([End Frame])
    EndFrame -->|Repeat| StartFrame
```

---

## Chi Tiết Chuỗi Gọi Hàm Qua Các File (Call Chain)

### 1. Luồng HandleInput
Chịu trách nhiệm dịch phím bấm thành logic game.

- **`src/states/BaseLevelState.cpp`** (`BaseLevelState::HandleInput`)
  - Gọi **`src/core/InputHandler.cpp`** (`InputHandler::handleInput`)
    - Kiểm tra phím cứng (raylib `IsKeyDown`).
    - Trả về danh sách `IPlayerCommand*`.
  - Duyệt qua mảng lệnh, gọi đa hình **`src/command/PlayerCommands.h`** (vd: `JumpCommand::Execute(Player& player)`).
    - Lệnh gọi **`src/entity/Player/Player.cpp`** (`Player::onJump()`).
      - Chuyển tiếp tới **`src/entity/Player/PlayerState.cpp`** (ví dụ `PlayerRunState::onJump()`).
      - Nếu thoả mãn, gọi `changePlayerState` để huỷ state cũ (gọi `onExit()`) và khởi tạo state mới (gọi `onEnter()`).

### 2. Luồng Process
Chịu trách nhiệm dọn dẹp, chuyển đổi trạng thái cục diện (không bị phụ thuộc physics time).

- **`src/states/BaseLevelState.cpp`** (`BaseLevelState::Process`)
  - Lấy các lệnh diện rộng từ **`include/command/CommandQueue.h`** (vd: `ExplosionDamage`).
  - Gây sát thương nổ (gọi **`src/entity/Effects.cpp`** `LavaEffect`).
  - Xoá các entity đã chết khỏi `activeEntities` và **`src/combatsystem/CombatSystem.cpp`** (`CombatSystem::removeInactive`).
  - Cập nhật Camera qua **`src/core/MapCamera.cpp`** (`MapCamera::Update`).
  - Kiểm tra nếu người chơi ra khỏi rìa map: gọi `TransitionToLevel`.
    - Xoá hết entities cũ.
    - Gọi **`src/environment/TileMap.cpp`** (`TileMap::LoadLDtkMap(newLevel)`).
    - Gọi **`src/entity/Item/ItemFactory.cpp`** và **`src/entity/EntityFactory.cpp`** để spawn đợt quái/item mới từ json LDtk của map mới.

### 3. Luồng Update
Cập nhật logic, vật lý, va chạm (Chạy với Fixed Timestep = 1/60s).

- **`src/states/BaseLevelState.cpp`** (`BaseLevelState::Update(dt)`)
  - **A. Vật lý người chơi:**
    - Gọi **`src/entity/Entity.cpp`** (`Player::updatePhysicsWithMap()`).
      - Gọi nội bộ `applyGravity(dt)`.
      - Gọi `resolveCollisionX()` và `resolveCollisionY()` bằng cách gọi **`src/environment/TileMap.cpp`** (`TileMap::GetCollidingTiles(hitbox)`).
      - Xử lý block đặc biệt (trigger liquid) bằng `handleTriggers()`. Nếu vào dung nham, gọi **`src/entity/Entity.cpp`** `addEffect(LavaEffect)`.
      - Xử lý callback nếu chạm đất/trần: `onLand()`, `onHitCeiling()`.
    - Gọi **`src/entity/Player/Player.cpp`** (`Player::updateStateFromPhysics()`) để tự rơi tự do nếu mất chân đứng (chuyển sang `PlayerFallState`).
    - Gọi `Player::update(dt)` → gọi `PlayerState::update(dt)` hiện tại. Nếu đang tung skill, gọi **`src/entity/Skill/ISkill.h`** (`ISkill::execute`).
  - **B. Vật lý Items & NPC:**
    - Gọi `BaseItem::update(dt)` từ **`src/entity/Item/BaseItem.cpp`**. (vd Boom thì đếm ngược nổ).
    - Vật lý các khối rơi tự do cũng được xử lý tương tự như Player.
  - **C. Va chạm Item - Player:**
    - Lặp qua mảng `activeItems`. Nếu hitbox người chơi đè lên `item->getHitbox()`:
      - Gọi đa hình **`src/entity/Item/...`** (vd `LuckyBlock::onInteract(Player)` hoặc `Coin::onInteract()`).
      - Gọi `CommandQueue::push(SpawnCommand)` nếu rơi ra đồ.
  - **D. Xử lý hàng đợi Spawn:**
    - Gọi **`include/command/CommandQueue.h`** `popAll()`.
    - Gửi thông số tới `ItemFactory::createDynamic()` hoặc `EntityFactory::create()`.
    - `push_back` item/entity mới vào mảng đang update (hoạt động ngay ở frame tiếp theo).
  - **E. Combat (Gây sát thương chém/đấm):**
    - Gọi **`src/combatsystem/CombatSystem.cpp`** (`CombatSystem::update()`).
    - Nó gọi **`src/combatsystem/BruteForceDetector.cpp`** để quét chéo các Hitbox. Trừ máu bằng `takeDamage()`.

### 4. Luồng Render
Vẽ mọi thứ ra màn hình.

- **`src/states/BaseLevelState.cpp`** (`BaseLevelState::Render(alpha)`)
  - Gọi **`src/core/MapCamera.cpp`** (`MapCamera::BeginMode()`).
  - Gọi **`src/environment/TileMap.cpp`** (`TileMap::Draw()`). Vẽ Background, vẽ Canvas các tile tĩnh.
  - Gọi `BaseItem::render(alpha)` trên tất cả item.
    - Ở trong **`src/entity/Item/BaseItem.cpp`**, gọi **`src/entity/Item/ItemAtlasRegistry.cpp`** (`getTexture()`, `getFrame()`) để lấy toạ độ ảnh spritesheet và vẽ ra.
  - Gọi `Entity::render(alpha)` trên tất cả NPCs và Players.
    - Gọi **`src/entity/Animation.cpp`** (`Animation::getCurrentFrame()`) để vẽ frame chớp liên tục.
  - Gọi `MapCamera::EndMode()`.
  - Vẽ UI (máu, coin) đè lên trên (không bị ảnh hưởng bởi camera).
