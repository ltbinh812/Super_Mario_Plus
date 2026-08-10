# Kế Hoạch Merge: binh01 → binh02

## Bối cảnh
| | binh02 (current — code của bạn) | binh01 (incoming) |
|---|---|---|
| **Điểm mạnh** | Skill System, Hitbox, Combat, Fireball, Combo | Camera, TileMap (LDtk), CollisionType Enum, SwimState, ClimbState |
| **Entity/Player** | `physicsBox` + `crouchBox` trong BaseStats; `Hitbox`, `findSkill`, `spawnFireball`, `takeDamage`, `hasActiveHitbox` | `hitbox` trong RuntimeStats; `collisionMask`, `ignoreOneWayTimer`, `ignoreLadderTimer`, `isOverlappingLadder`; hooks `onEnterWater`, `onOverlapLadder`, `onHazard`, `onDie`, `dropThrough` |
| **TileMap** | `GetCollidingRectangles()` → `vector<Rectangle>` | `GetCollidingTiles()` → `vector<CollisionTile>` với `CollisionType enum` |
| **Camera** | `MapCamera` cũ, thiếu `SetMapTileSize` | `MapCamera` mới hoàn thiện, co-op zoom |

---

## Quy tắc Merge
- **Di chuyển, hitbox, skill, combat** → Giữ code binh02
- **Camera, TileMap, map loading, CollisionType** → Lấy code binh01
- **CharacterStats, Entity, Player** → **Merge thủ công**: lấy cái tốt nhất từ 2 bên, đảm bảo cả 2 hệ thống cùng hoạt động

---

## Phân Tích Chi Tiết Từng File

### NHÓM 1: Lấy TOÀN BỘ từ binh01 (không conflict lớn)
Các file này chỉ có ở binh01, hoặc binh02 chưa có:
- `include/environment/MapCamera.h` → lấy binh01 (camera mới)
- `src/environment/MapCamera.cpp` → lấy binh01 (camera mới)
- `src/environment/TileMap.cpp` → lấy binh01 (LDtk smart path, GetCollidingTiles)
- `include/environment/TileMap.h` → lấy binh01 (CollisionType enum, CollisionTile struct, GetCollidingTiles)
- `include/entity/Player/PlayerSwimState.h` + `.cpp` → NEW từ binh01
- `include/entity/Player/PlayerClimbState.h` + `.cpp` → NEW từ binh01
- `src/states/World01State.cpp`, `World02State.cpp`, `World03State.cpp` → lấy binh01 (camera mới, multi-player setup)
- `workflow/blueprints/*.md` từ binh01 → giữ cả 2 bên

### NHÓM 2: Giữ code binh02 (hoặc bỏ qua phần binh01 ghi đè)
- `src/entity/Player/PlayerSkillState.cpp` → giữ binh02 (combo system, hasExecuted logic)
- `src/entity/Player/PlayerRunState.cpp` → giữ binh02 (gọi useSkill "Punch1" khi attack)
- `src/entity/Fireball.cpp` + `include/entity/Fireball.h` → giữ binh02 (không có trong binh01)
- `include/entity/Skill/*` → giữ binh02 (ISkill, Punch1..4, DashSkill, LongAttack, Block — không có trong binh01)
- `include/entity/EntityFactory.h` + `.cpp` → giữ binh02

### NHÓM 3: MERGE THỦ CÔNG (File bị conflict thực sự)

#### `include/entity/Player/CharacterStats.h` — **CONFLICT QUAN TRỌNG**
| binh02 | binh01 |
|---|---|
| `physicsBox` trong BaseStats | không có `physicsBox` trong BaseStats |
| `crouchBox` trong BaseStats | không có `crouchBox` |
| `physicsBox` trong RuntimeStats | `hitbox` trong RuntimeStats |
| Thiếu `collisionMask`, `ignoreOneWayTimer`, `isOverlappingLadder` | Có `collisionMask`, `ignoreOneWayTimer`, `ignoreLadderTimer`, `isOverlappingLadder` |
| Thiếu `#include <cstdint>` | Có `#include <cstdint>` |
| `previousPos` trong WorldStats | không có |

**→ Quyết định:** Merge lại theo hướng **giữ cả 2**, vì:
- `physicsBox` (BaseStats) dùng để khởi tạo hitbox khi crouch/stand — cần giữ
- `hitbox` (RuntimeStats từ binh01) là hitbox đang hoạt động — cần giữ
- `collisionMask`, `ignoreOneWayTimer` — cần thêm vào

**→ Kết quả cuối:**
```cpp
struct CharacterBaseStats {
    std::string name;
    int maxHealth, maxMana;
    float moveVelocity, jumpVelocity, gravityScale;
    Vector2 physicsBox;   // Base hitbox (đứng thẳng)
    Vector2 crouchBox;    // Hitbox khi cúi người
};

struct CharacterRuntimeStats {
    int health, mana;
    // Combat
    Vector2 physicsBox;              // Hitbox hiện tại (thay đổi khi crouch)
    Vector2 velocity;
    bool isGrounded = false;
    bool isOverlappingLadder = false;
    uint32_t collisionMask = 0xFFFFFFFF;
    float ignoreOneWayTimer = 0.0f;
    float ignoreLadderTimer = 0.0f;
};
```
*(Bỏ `hitbox` trùng lặp của binh01, dùng `physicsBox` nhất quán; bỏ `attack/defense` raw vì đã có trong ISkill/Hitbox system)*

---

#### `include/entity/Entity.h` — **CONFLICT**
| binh02 | binh01 |
|---|---|
| `hasActiveHitbox()`, `getActiveHitbox()`, `takeDamage()` (combat) | `dropThrough()`, `onEnterWater()`, `onOverlapLadder()`, `onHazard()`, `onDie()` hooks |
| `commandQueue` pointer (SpawnCommand) | không có |
| `setCommandQueue()`, `getIsActive()` | không có |
| `getHitbox()` dùng `physicsBox` | `getHitbox()` dùng `hitbox` |

**→ Quyết định:** Hợp nhất cả 2, lấy tất cả virtual hooks từ binh01, giữ combat API từ binh02.

---

#### `src/entity/Entity.cpp` — **CONFLICT**
- binh02: Dùng `GetCollidingRectangles()` (old API) + `physicsBox`
- binh01: Dùng `GetCollidingTiles()` (new API) + `collisionMask` + trigger hooks

**→ Quyết định:** Lấy TOÀN BỘ logic của binh01 (vì physics engine mạnh hơn nhiều), chỉ sửa `hitbox` → `physicsBox` để nhất quán với tên field.

---

#### `include/entity/Player/Player.h` — **CONFLICT**
| binh02 | binh01 |
|---|---|
| `findSkill()`, `hasEnoughMana()`, `spawnFireball()` | `onClimb()`, `onEnterWater()`, `onHazard()`, `onDie()` |
| `hasActiveHitbox()`, `getActiveHitbox()`, `takeDamage()` | `swimState`, `climbState` |
| `Hitbox.h` include | không có |

**→ Quyết định:** Merge thủ công — lấy tất cả, thêm swimState/climbState vào binh02.

---

#### `src/entity/Player/Player.cpp` — **CONFLICT**
| binh02 | binh01 |
|---|---|
| `crouch()` reset physicsBox | `crouch()` chỉ reset velocity |
| `hasActiveHitbox()`, `getActiveHitbox()` | thiếu |
| `takeDamage()` | thiếu |
| `spawnFireball()` | thiếu |
| `updateStateFromPhysics()` không check swim/climb | check swim/climb trước |
| `onCrouch()` chỉ gọi state | `onCrouch()` gọi `dropThrough()` + climb check |

**→ Quyết định:** Merge thủ công — base từ binh01 (có swim/climb/hooks), **giữ lại** combat methods từ binh02.

---

#### `include/entity/Player/PlayerStates.h` — **CONFLICT**
- binh01 có thêm: `#include "PlayerSwimState.h"`, `#include "PlayerClimbState.h"`
- **→ Lấy binh01**

---

#### `include/entity/Player/PlayerSkillState.h` + `.cpp`
- binh01 có `PunchSkill` (1 skill thay thế Punch1-4), thiếu `nextSkill`, `hasExecuted` logic
- binh02 có full combo chain, `nextSkill`, `hasExecuted`, `isHitboxActive()`
- **→ Giữ binh02 HOÀN TOÀN**

---

#### `src/entity/Player/PlayerIdleState.cpp`, `PlayerFallState.cpp`, `PlayerJumpState.cpp`
- binh01 thêm `updateStateFromPhysics` vào trong State (nhưng binh02 chưa)
- **→ Giữ binh02, không merge phần state-driven physics của binh01** (vì binh02 vẫn dùng external `updateStateFromPhysics()`, đã được ghi lại là anti-pattern nhưng chưa refactor trong sprint này)

---

#### `CMakeLists.txt`
- binh01 có thêm `PlayerSwimState.cpp`, `PlayerClimbState.cpp`
- **→ Merge: lấy binh01 + thêm lại các file của binh02** như `Fireball.cpp`, `EntityFactory.cpp`, Punch skills...

---

## Thứ Tự Thực Hiện

1. Chạy `git merge origin/binh01` để bắt đầu
2. Sửa **`CharacterStats.h`** trước (vì mọi file khác đều phụ thuộc)
3. Sửa **`Entity.h`** + **`Entity.cpp`** (cập nhật `physicsBox` API mới)
4. Sửa **`Player.h`** + **`Player.cpp`** (merge combat + swim/climb)
5. Sửa **`CMakeLists.txt`** (thêm file mới)
6. Checkout các file của binh01/binh02 theo quy tắc nhóm 1 & 2
7. Commit

> [!IMPORTANT]
> Sau bước 2, cần đổi `runtimeStats.hitbox` → `runtimeStats.physicsBox` trong **toàn bộ Entity.cpp** để tránh compile error.

> [!WARNING]
> `PlayerFactory.cpp` dùng `physicsBox` và `crouchBox` trong BaseStats. Đảm bảo binh01 PlayerFactory không xóa các field này.

## Verification Plan
- Build project sau khi merge → không có compile error
- Chạy World01: Player di chuyển, nhảy, dùng skill bình thường
- Chạy World02 (nếu có): Camera zoom đúng, map load đúng
- Kiểm tra Swim/Climb state (nếu có water/ladder trong map)
