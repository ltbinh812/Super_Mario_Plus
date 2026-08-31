# Blueprint 2: Function Interaction & Command Patterns

Tài liệu này mô tả các sequence diagram và flow diagram cho các tương tác phức tạp giữa Player, Map, Item và các cơ chế vật lý (nước, lava, poison).

### Phân vai tương tác (Roles)
- **`BaseLevelState`**: Nơi khởi nguồn các tương tác. Nó điều khiển vòng lặp Update/Process, trực tiếp quét va chạm giữa các vật thể, đưa ra quyết định dựa trên Command, và chứa tất cả đối tượng tham gia tương tác.
- **`TileMap`**: Kho lưu trữ thông tin không gian. Nó được hỏi liên tục mỗi khi một Entity di chuyển để trả lời câu hỏi *"Tại toạ độ này, có gặp vật thể rắn, nước, dung nham hay không?"* (`GetCollidingTiles`).

---

## 1. Map Collision & Liquid Parsing (LDtk → TileMap)

Cách mà code lấy được thông số các block rắn, lỏng từ LDtk và lưu vào TileMap:

```mermaid
sequenceDiagram
    participant Process as BaseLevelState::Process
    participant TileMap
    participant LDtk as File .ldtk
    
    Process->>TileMap: LoadLDtkMap("world.ldtk")
    TileMap->>LDtk: Read JSON
    
    Note over TileMap: 1. Parse Layers & Collision
    TileMap->>TileMap: Parse "defs.layers.intGridValues"
    TileMap->>TileMap: Map {1: Solid, 2: Water, 3: Lava, 4: Poison, ...}
    
    TileMap->>TileMap: Read "Collision" layer instance
    TileMap->>TileMap: populate collisionLayer[y][x] with CollisionType
    
    Note over TileMap: 2. Parse Entities (Items, Spring, etc)
    TileMap->>TileMap: Read "Entities" layer instance
    TileMap->>TileMap: Collect entityData_ {id: "Spring_down", px: (x,y)}
```

---

## 2. Input → Command → Player Action

Luồng xử lý từ khi người chơi bấm phím đến khi Player thực hiện hành động (đổi State).

```mermaid
sequenceDiagram
    participant GameLoop as Game::Update
    participant Input as InputHandler
    participant Cmd as IPlayerCommand
    participant Player as Player
    participant State as PlayerState
    
    GameLoop->>Input: handleInput()
    Input-->>GameLoop: vector<IPlayerCommand*>
    
    loop For each command
        GameLoop->>Cmd: Execute(player)
        Cmd->>Player: e.g. onJump()
        Player->>State: onJump()
        
        opt If state allows jumping
            State->>Player: changePlayerState(jumpState)
            Player->>State: oldState->onExit()
            Player->>State: newState->onEnter()
        end
    end
```

---

## 3. Physics & Environment Interaction (Liquid, Lava, Poison)

Cách Player xử lý va chạm với môi trường (rắn, lỏng, độc, lửa) mỗi frame.

```mermaid
sequenceDiagram
    participant Update as BaseLevelState::Update
    participant Player as Entity (Player)
    participant TileMap
    participant Effect as IEffect (Poison/Lava)
    
    Update->>Player: updatePhysicsWithMap(map, dt)
    Player->>Player: applyGravity()
    
    Note over Player: 1. Xử lý va chạm vật lý (Solid/OneWay)
    Player->>TileMap: GetCollidingTiles()
    TileMap-->>Player: vector<CollisionTile>
    Player->>Player: resolveCollisionX()
    Player->>Player: resolveCollisionY()
    
    Note over Player: 2. Xử lý Trigger (Liquid, Lava, Hazard)
    Player->>Player: handleTriggers(map, dt)
    Player->>TileMap: GetCollidingTiles(triggerHitbox)
    TileMap-->>Player: vector<CollisionTile> (e.g. Water, Lava, Poison)
    
    alt In Liquid (Water/Poison/Lava)
        Player->>Player: runtimeStats.currentLiquid = dominantLiquid
        Player->>Player: onEnterWater() → PlayerSwimState
        opt If dominantLiquid == Poison
            Player->>Player: addEffect(PoisonEffect)
        end
        opt If dominantLiquid == Lava
            Player->>Player: addEffect(LavaEffect)
            Player->>Player: Update active LavaEffect: setInLava(true)
        end
    else Not in liquid
        Player->>Player: onExitLiquid() → Fall/Idle state
        Player->>Player: Update active LavaEffect: setInLava(false)
    end
    
    Note over Player: 3. Xử lý Effect (mỗi frame)
    Player->>Player: updateEffects(dt)
    Player->>Effect: update(entity, dt)
    opt Timer tick
        Effect->>Player: takeDamage(1)
    end
```

---

## 4. Item Interaction Flow (Spring, Coins, Chests, LuckyBlock)

Khi Player va chạm với các item, cách các hàm được gọi.

```mermaid
sequenceDiagram
    participant Update as BaseLevelState::Update
    participant Player
    participant Item as BaseItem (Spring/Coin/LuckyBlock)
    participant Factory as ItemFactory
    participant Queue as CommandQueue
    
    Update->>Update: Check Player Hitbox intersects Item Hitbox
    Update->>Item: onInteract(player)
    
    alt Spring
        Item->>Player: vel.y = -LAUNCH_FORCE
        Item->>Item: triggered = true, play animation
        
    else Coin / Key
        Item->>Item: itemState = Used
        Item->>Player: partyInventory.coins++
        
    else ChestNormal / ChestBoss
        Item->>Item: if Idle → set Used, play 50% alpha frame
        Item->>Queue: push SpawnCommand{Buff/Boom/Key, pos}
        
    else LuckyBlock
        Item->>Item: check if player is jumping up from below
        opt Player hit bottom of block
            Item->>Item: isTriggered = true (solid block changes to empty/used)
            Item->>Queue: push SpawnCommand{Coin/Buff/Boom, pos}
            Item->>Player: vel.y = 0 (bonk head)
        end
        
    else Buff / Boom (Pickup)
        opt Inventory Slot is Empty
            Item->>Item: itemState = Used
            Item->>Player: storedItemSlot = "Boom"
        else Inventory Slot is Full
            Item->>Player: setOverlappingItem(this) (waits for Q to swap)
        end
    end
    
    Note over Update: Xử lý Spawn Item từ rương/block
    Update->>Queue: popAll()
    Queue-->>Update: vector<SpawnCommand>
    Update->>Factory: createDynamic(cmd)
    Factory-->>Update: new Item added to activeItems
```

---

## 5. Throw Boom & Explosion Pattern

Mô tả cơ chế ném vật phẩm và hàng đợi Command (Event-like) cho vùng sát thương (ExplosionDamage).

```mermaid
sequenceDiagram
    participant Player as Player
    participant Queue as CommandQueue
    participant Update as BaseLevelState::Update
    participant Boom as Boom (Thrown)
    participant Process as BaseLevelState::Process
    participant Target as Entity (Enemy/Player)
    
    Note over Player: 1. Ném Boom (Input: Q)
    Player->>Player: throwStoredItem()
    Player->>Queue: push SpawnCommand{ThrownBoom, vel={±350, -280}}
    
    Note over Update: 2. Sinh Boom vào Game (Frame N)
    Update->>Queue: popAll()
    Update->>ItemFactory: createDynamic("ThrownBoom", pos, vel)
    ItemFactory-->>Update: new Boom(pos, vel)
    Update->>Boom: activate()
    
    Note over Boom: 3. Vật lý & Đếm ngược (Frame N+1... M)
    loop Every Frame
        Update->>Boom: update(dt)
        Boom->>Boom: timer_ -= dt
        opt timer <= 0
            Boom->>Boom: exploded_ = true
            Boom->>Queue: push SpawnCommand{ExplosionDamage, rect=5x3 blocks}
        end
    end
    
    Note over Process: 4. Gây Sát thương nổ (Frame M)
    Process->>Queue: peekAndConsumeByCategory(ExplosionDamage)
    loop For each entity in activeEntities + Players
        Process->>Process: CheckCollisionRecs(entity.hitbox, explosionRect)
        opt Colliding
            Process->>Target: addEffect(LavaEffect)
        end
    end
```
