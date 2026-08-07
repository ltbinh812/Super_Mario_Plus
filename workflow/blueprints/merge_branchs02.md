# Kế Hoạch Merge: binh01 → binh02 (OOP & Design Pattern Full Audit)

## Bối cảnh
| | binh02 (current) | binh01 (incoming) |
|---|---|---|
| **Điểm mạnh** | Skill System, Hitbox, Combat, Fireball, Combo | Camera, TileMap (LDtk), CollisionType, SwimState, ClimbState |
| **Điểm yếu** | Physics đơn giản, thiếu CollisionType | SwimState/ClimbState chưa dùng helper method của Player, trọng lực dùng raw `getRuntimeStatsMutable()` |

---

## PHẦN I — KIỂM TRA OOP & DESIGN PATTERN

### ❌ Vấn đề 1: `IEntityState<T>` — Interface quá yếu (vi phạm ISP)
**Hiện tại:**
```cpp
class IEntityState<T> {
    virtual void update(float dt) = 0;
    virtual void onEnter() = 0;
    virtual void onExit() = 0;
};
```
**Vấn đề:** Interface chỉ có 3 hàm pure virtual. Tất cả các hàm input như `onMoveLeft`, `onJump`, `onAttack`, `canExit` đều nằm trong `PlayerState` cụ thể — không phải trong interface chung. Điều này có nghĩa là:
- Không thể có `EnemyState` hoặc `NPCState` reuse interface mà không copy-paste.
- Vi phạm **Open/Closed Principle**: muốn thêm hook phải sửa `PlayerState`.

**→ Fix trong merge:** Mở rộng `IEntityState<T>` thành interface đủ dùng:
```cpp
template<typename T>
class IEntityState {
public:
    virtual ~IEntityState() = default;
    virtual void onEnter() = 0;
    virtual void onExit() = 0;
    virtual void update(float dt) = 0;
    virtual bool canExit() const { return true; }
    // Input hooks — default no-op (không phải pure virtual, nên không cần implement ở State không dùng)
    virtual void onMoveLeft() {}
    virtual void onMoveRight() {}
    virtual void onJump() {}
    virtual void onCrouch() {}
    virtual void onAttack() {}
    virtual void onStopLeft() {}
    virtual void onStopRight() {}
    virtual void onStopCrouch() {}
    virtual void onClimb() {}
};
```
Khi đó `PlayerState` bỏ đi việc re-khai báo lại tất cả các hàm này, chỉ cần kế thừa từ `IEntityState<Player>`.

---

### ❌ Vấn đề 2: `PlayerState` — Friend Class Không Cần Thiết
**Hiện tại:**
```cpp
class Player : public Entity {
    friend class PlayerState; // Mở toàn bộ private cho PlayerState
};
```
**Vấn đề:** `friend` là một trong những dấu hiệu xấu nhất về encapsulation trong C++. Vì `PlayerState` là friend, bất kỳ subclass nào của `PlayerState` cũng có thể gọi `Player::changeState()` trực tiếp — điều này đúng nhưng **nguy hiểm và không kiểm soát được**.

Thực tế đúng hơn là dùng `protected` hoặc một API tường minh.

**→ Fix trong merge:** Xóa `friend class PlayerState`. Thay vào đó:
- `changePlayerState()` trong `PlayerState` gọi `player.requestState()` (public) thay vì `player.changeState()` (private).
- `changeState()` private dùng nội bộ trong Player khi cần force-change (ví dụ takeDamage → hurtState).

---

### ❌ Vấn đề 3: `SwimState` & `ClimbState` — Vi phạm Tell, Don't Ask
**Hiện tại (binh01):**
```cpp
// Trong SwimState::onMoveLeft():
player.getRuntimeStatsMutable().velocity.x = -player.getBaseStats().moveVelocity * 0.7f;

// Trong ClimbState::update():
player.getRuntimeStatsMutable().velocity.y = player.getBaseStats().moveVelocity * 0.000001f;
```
**Vấn đề:** State đang lấy trực tiếp `getRuntimeStatsMutable()` để sửa dữ liệu nội bộ của Player — đây là vi phạm "Tell, Don't Ask" và đánh bại hoàn toàn mục đích của Encapsulation. State phải **ra lệnh** cho Player, không được **tự tay sửa** dữ liệu của Player.

**→ Fix trong merge:** Thêm các helper method vào `Player` để State gọi thay vì trực tiếp chỉnh stats:
```cpp
// Thêm vào Player.h:
void swim(float dirX);      // Bơi với hệ số giảm tốc, cập nhật velocity.x
void climbUp();             // Cập nhật velocity.y âm
void climbDown();           // Cập nhật velocity.y dương
void stopClimb();           // Đặt velocity = {0, nhỏ}
void setGravityOverride(float scale); // Override gravityScale tạm thời cho swim
```
Sau đó:
```cpp
// SwimState::onMoveLeft():
player.swim(-1.0f);   // Sạch, rõ ràng
// ClimbState::onClimb():
player.climbUp();
```

---

### ❌ Vấn đề 4: `PlayerFactory` — Factory không theo chuẩn (if-else chain)
**Hiện tại:**
```cpp
if (skillName == "Dash") {
    skill = std::make_unique<DashSkill>();
} else if (skillName == "Punch1") {
    skill = std::make_unique<Punch1Skill>();
} // ...12 dòng if-else tiếp theo
```
**Vấn đề:** Đây là **anti-pattern** điển hình. Mỗi khi thêm skill mới, bạn phải sửa `PlayerFactory.cpp` — vi phạm **Open/Closed Principle**. Với 20 skill sẽ có 20 if-else.

**→ Fix trong merge:** Dùng `std::unordered_map` với factory lambdas:
```cpp
// Trong PlayerFactory.cpp (static hoặc tạo một lần):
static const std::unordered_map<std::string, std::function<std::unique_ptr<ISkill>()>> skillRegistry = {
    { "Dash",        []{ return std::make_unique<DashSkill>(); } },
    { "Block",       []{ return std::make_unique<BlockSkill>(); } },
    { "Punch1",      []{ return std::make_unique<Punch1Skill>(); } },
    { "Punch2",      []{ return std::make_unique<Punch2Skill>(); } },
    { "Punch3",      []{ return std::make_unique<Punch3Skill>(); } },
    { "Punch4",      []{ return std::make_unique<Punch4Skill>(); } },
    { "LongAttack",  []{ return std::make_unique<LongAttackSkill>(); } },
};

// Dùng:
auto it = skillRegistry.find(skillName);
if (it != skillRegistry.end()) {
    skill = it->second();
} else {
    std::cerr << "Unknown skill: " << skillName << std::endl;
    continue;
}
```
Khi thêm skill mới chỉ cần thêm 1 dòng vào `skillRegistry`, không cần sửa logic.

---

### ❌ Vấn đề 5: `updateStateFromPhysics()` — Anti-Pattern FSM (đã biết, cần quyết định)
**Hiện tại:** Gọi từ bên ngoài mỗi frame trong WorldState:
```cpp
player->updateStateFromPhysics();
player->update(dt);
```
**Vấn đề (đã nêu trong entity evaluation):** Logic chuyển state từ physics thuộc về **trong State**, không phải từ ngoài vào. Tuy nhiên, đây là refactor lớn — không nên làm trong sprint merge này.

**→ Quyết định cho sprint này:** **Giữ nguyên** nhưng cải thiện:
- `updateStateFromPhysics()` phải **không được gọi** khi Player đang ở `skillState`, `swimState`, `climbState`, `hurtState`, `dieState`.
- Sửa lại check trong `Player::updateStateFromPhysics()` để đủ điều kiện.

```cpp
void Player::updateStateFromPhysics() {
    // Không can thiệp khi đang ở state tự quản lý
    if (currentState == &skillState ||
        currentState == &swimState  ||
        currentState == &climbState ||
        currentState == &hurtState  ||
        currentState == &dieState)
        return;
    // ... logic cũ
}
```
*(Refactor toàn diện thành State-Driven Physics là Phase 3 riêng.)*

---

### ❌ Vấn đề 6: `const_cast` trong `getActiveHitbox()` — Code Smell Nguy Hiểm
**Hiện tại (binh02):**
```cpp
return { worldRect, skill->getAttackPower(), skill->getDefensePower(),
         const_cast<Entity*>(static_cast<const Entity*>(this)) };
```
**Vấn đề:** `const_cast` từ `const Player*` sang `Entity*` vi phạm const-correctness. Nếu `Hitbox.owner` sau này bị dùng để gọi một hàm non-const, đây là Undefined Behavior.

**→ Fix trong merge:** Đổi `owner` trong struct `Hitbox` thành `Entity* owner` (non-const), và bỏ `const` khỏi hàm `getActiveHitbox()` ở Player:
```cpp
// Entity.h:
virtual Hitbox getActiveHitbox() { return {{0,0,0,0}, 0, 0, nullptr}; }  // non-const

// Player.cpp:
Hitbox Player::getActiveHitbox() {   // non-const
    // ...
    return { worldRect, skill->getAttackPower(), skill->getDefensePower(), this };  // clean
}
```

---

### ⚠️ Vấn đề 7: `Entity` constructor nhận `const&` ở binh02, nhận `&` ở binh01 — Không nhất quán
**binh02:** `Entity(const CharacterBaseStats& bS, ...)` ✅
**binh01:** `Entity(CharacterBaseStats& bS, ...)` ❌ (mutable ref không cần thiết)

**→ Fix:** Dùng `const&` nhất quán theo binh02.

---

### ⚠️ Vấn đề 8: `PlayerClimbState::update()` — Magic Number cực xấu
**Hiện tại (binh01):**
```cpp
player.getRuntimeStatsMutable().velocity.y = player.getBaseStats().moveVelocity * 0.000001f;
```
Đây là hack tạm thời để "giả vờ đứng yên" — số `0.000001f` không có ý nghĩa gì và sẽ gây bug physics sau này.

**→ Fix:** Khi merge, thay bằng `player.stopClimb()` hoặc đơn giản là `velocity.y = 0.0f` qua helper method.

---

### ✅ Điểm tốt — Giữ nguyên
- **Strategy Pattern** (`ISkill`) — tốt, mỗi skill là một class độc lập
- **Command Pattern** (`PlayerCommands`) — đúng chuẩn, Input → Command → Execute(Entity)
- **Factory Pattern** (`EntityFactory`, `PlayerFactory`) — đúng hướng (chỉ cần fix if-else)
- **Flyweight Pattern** trong `TileMap` (binh01) — tốt, dùng chung texture theo uid
- **Observer/Hook** (`onEnterWater`, `onOverlapLadder`) trong binh01 — tốt, mở rộng được
- **Bitmask Collision Mask** trong binh01 — tốt hơn nhiều boolean flags

---

## PHẦN II — KẾ HOẠCH MERGE (Cập nhật)

### Quy tắc
- Camera, TileMap, CollisionType → lấy binh01
- Skill, Hitbox, Combat, Fireball → giữ binh02
- CharacterStats, Entity, Player → merge thủ công + áp dụng các fix OOP ở trên

### Thứ Tự Thực Hiện

**Bước 0:** `git merge origin/binh01` (bắt đầu merge, để conflict xảy ra)

**Bước 1 — `IEntityState.h`** (sửa trước mọi thứ):
- Mở rộng interface theo fix Vấn đề 1
- `PlayerState.h`: bỏ re-khai báo các hàm đã có trong interface

**Bước 2 — `CharacterStats.h`** (merge thủ công):
- Giữ `physicsBox` + `crouchBox` trong `BaseStats` (từ binh02)
- Giữ `physicsBox` trong `RuntimeStats` (từ binh02, đổi tên cho nhất quán)
- Thêm `collisionMask`, `ignoreOneWayTimer`, `ignoreLadderTimer`, `isOverlappingLadder` (từ binh01)
- Thêm `#include <cstdint>`
- Bỏ `attack`/`defense` raw fields trong RuntimeStats (không dùng, đã có trong ISkill)

**Bước 3 — `TileMap.h` + `TileMap.cpp`**: Lấy toàn bộ binh01 (CollisionType, GetCollidingTiles, Smart path, Auto level fallback)

**Bước 4 — `Entity.h`** (merge thủ công):
- Giữ: `commandQueue`, `hasActiveHitbox()`, `getActiveHitbox()`, `takeDamage()`, `getIsActive()`, `deactivate()`, `onCollide()`, `setCommandQueue()` (từ binh02)
- Thêm: `dropThrough()`, `onEnterWater()`, `onOverlapLadder()`, `onHazard()`, `onDie()` (từ binh01)
- Fix constructor: dùng `const&` nhất quán
- Fix `getActiveHitbox()`: bỏ `const` (fix Vấn đề 6)

**Bước 5 — `Entity.cpp`** (lấy logic binh01, patch lại):
- Lấy toàn bộ physics engine mới của binh01 (bitmask, triggers, OneWay/Lotus)
- Đổi `runtimeStats.hitbox` → `runtimeStats.physicsBox` (nhất quán với binh02)

**Bước 6 — `Player.h`** (merge thủ công):
- Giữ tất cả binh02: `findSkill()`, `hasEnoughMana()`, `spawnFireball()`, `hasActiveHitbox()`, `getActiveHitbox()`, `takeDamage()`
- Thêm từ binh01: `swimState`, `climbState`, `onClimb()`, `onEnterWater()`, `onOverlapLadder()`, `onHazard()`, `onDie()`
- Bỏ `friend class PlayerState` (fix Vấn đề 2)
- **Thêm mới** các helper method: `swim(float dir)`, `climbUp()`, `climbDown()`, `stopClimb()` (fix Vấn đề 3)

**Bước 7 — `Player.cpp`** (merge thủ công):
- Base: lấy binh01 (có swim/climb hooks hoàn chỉnh)
- Giữ lại từ binh02: `findSkill()`, `hasEnoughMana()`, `hasActiveHitbox()`, `getActiveHitbox()` (sửa const_cast), `takeDamage()`, `spawnFireball()`
- Sửa `crouch()`: giữ logic reset `physicsBox` từ binh02 (binh01 đã mất)
- Sửa `updateStateFromPhysics()`: thêm guard cho `skillState`, `swimState`, `climbState` (fix Vấn đề 5)
- **Implement** các helper mới: `swim()`, `climbUp()`, `climbDown()`, `stopClimb()`
- Sửa `onMoveLeft/Right` trong `PlayerState.changePlayerState()`: dùng `player.requestState()` thay vì `player.changeState()`

**Bước 8 — `PlayerSwimState.cpp` + `PlayerClimbState.cpp`**: Lấy từ binh01, nhưng **refactor** theo fix Vấn đề 3 (dùng helper methods thay vì `getRuntimeStatsMutable()`)

**Bước 9 — `PlayerFactory.cpp`**: Giữ binh02, **refactor** if-else → map registry (fix Vấn đề 4)

**Bước 10 — `MapCamera.h` + `MapCamera.cpp`**: Lấy toàn bộ binh01

**Bước 11 — `World01State.cpp`, `World02State.cpp`, `World03State.cpp`**: Lấy binh01 (camera mới), patch lại các thay đổi về Entity/Player API nếu cần

**Bước 12 — `PlayerStates.h`**: Lấy binh01 (có thêm SwimState, ClimbState includes)

**Bước 13 — `CMakeLists.txt`**: Merge — giữ tất cả file của binh02 + thêm `PlayerSwimState.cpp`, `PlayerClimbState.cpp` của binh01

**Bước 14 — `git commit`**

---

## PHẦN III — VẤN ĐỀ ĐỂ LẠI CHO PHASE SAU (KHÔNG LÀM TRONG SPRINT NÀY)

| # | Vấn đề | Lý do để lại |
|---|---|---|
| A | Refactor `updateStateFromPhysics()` ra khỏi external call → State-Driven Physics | Ảnh hưởng tất cả states, cần sprint riêng |
| B | Đổi `friend class PlayerState` sang API `protected` hoàn chỉnh | Cần redesign `changePlayerState()` |
| C | Bỏ `const_cast` trong `getActiveHitbox()` | Nhỏ, làm luôn trong sprint này |
| D | Thêm `EnemyState` system tương tự PlayerState | Feature mới |
| E | `EntityFactory` switch-case → registry pattern tương tự skill | Nhỏ, để sau |

---

## Verification Plan
1. Build không có lỗi compile
2. World01: Player di chuyển, nhảy, dùng Punch combo, Dash bình thường
3. World01: Đánh trúng player 2 → `takeDamage()` hoạt động
4. World02/03 (nếu có ladder/water): SwimState, ClimbState kích hoạt đúng
5. OneWay platform: nhảy lên được, không lọt qua; bấm xuống thì rớt qua
6. Camera zoom co-op: khi 2 player cách xa nhau thì zoom out
