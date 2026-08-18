# Blueprint 1: Class Hierarchy & Composition

Sơ đồ kế thừa (inheritance) và sở hữu (composition/aggregation) của toàn bộ class trong dự án.

---

## 1. Entity Hierarchy

```mermaid
classDiagram
    direction TB

    class Entity {
        <<abstract>>
        #CharacterBaseStats baseStats
        #CharacterRuntimeStats runtimeStats
        #CharacterWorldStats worldStats
        #CommandQueue* commandQueue
        #vector~IEffect~ activeEffects
        +update(dt)* void
        +render(alpha)* void
        +updatePhysicsWithMap(map, solids, dt) void
        +getHitbox() Rectangle
        +takeDamage(damage) void
        +addEffect(IEffect) void
        +onLand(floorY) void
        +onHitCeiling(ceilY) void
        +onEnterWater() void
        +onOverlapLadder() void
        +onHazard() void
        +onDie() void
    }

    class Player {
        -PlayerState* currentState
        -map~ISkill~ skillList
        -BaseItem* overlappingItem_
        -shared_ptr~PartyInventory~ partyInventory_
        +10 PlayerState instances (owns)
        +onMoveLeft/Right/Jump/Attack()
        +interactWithOverlapping()
        +throwStoredItem()
        +useSkill(name)
    }

    class Fireball {
        -float speed
        -bool isFacingRight
        +update(dt)
        +render(alpha)
    }

    class BaseItem {
        <<abstract>>
        #ItemState itemState_
        #float pickupDelay_
        #float animTimer_
        #float hitW_, hitH_
        +update(dt) void
        +onInteract(Entity&) virtual
        +forceInteract(Entity&) virtual
        +getHitbox() Rectangle
        +getSolidRect() Rectangle
        +getRenderOffsetY() float
        +drawFrame(name, tint)
        +setPickupDelay(delay)
    }

    Entity <|-- Player
    Entity <|-- Fireball
    Entity <|-- BaseItem
```

---

## 2. Item Subclasses

```mermaid
classDiagram
    direction LR

    class BaseItem {
        <<abstract>>
    }

    class Spring {
        -SpringDir dir_
        -bool triggered_
        +getSolidRect() Rectangle
        +getHitbox() Rectangle
        +onInteract(Entity&) : vel += LAUNCH_FORCE
    }

    class Coin {
        +onInteract(Entity&) : coins++
    }

    class Key {
        +onInteract(Entity&) : keys++
    }

    class Door {
        -bool isOpen
        +getSolidRect() Rectangle
        +onInteract(Entity&) : requires Key
    }

    class Flag {
        -float animTimer_
        -bool frameToggle_
        +onInteract(Entity&) : win trigger
    }

    class LuckyBlock {
        -bool isTriggered
        +getSolidRect() Rectangle
        +getHitbox() Rectangle (expanded +2px)
        +onInteract(Entity&) : playerTop~blockBottom → spawn
    }

    class ChestNormal {
        -string frame_
        +onInteract(Entity&) : random Buff or Boom
    }

    class ChestBoss {
        -string frame_
        +onInteract(Entity&) : drop Key
    }

    class Buff {
        -unique_ptr~IBuffEffect~ effect_
        +onInteract(Entity&) : storedItemSlot = "Buff"
        +forceInteract(Entity&) : swap slot
    }

    class Boom {
        -bool active_, exploded_
        -bool damageEmitted_
        -float timer_, animTimer_
        +activate()
        +update(dt) : countdown → ExplosionDamage cmd
        +onInteract(Entity&) : storedItemSlot = "Boom"
        +throwStoredItem() via Player
        +getExplosionRect() Rectangle
    }

    BaseItem <|-- Spring
    BaseItem <|-- Coin
    BaseItem <|-- Key
    BaseItem <|-- Door
    BaseItem <|-- Flag
    BaseItem <|-- LuckyBlock
    BaseItem <|-- ChestNormal
    BaseItem <|-- ChestBoss
    BaseItem <|-- Buff
    BaseItem <|-- Boom
```

---

## 3. Player State Machine

```mermaid
classDiagram
    direction LR

    class IEntityState~T~ {
        <<interface>>
        +onEnter()* void
        +onExit()* void
        +update(dt)* void
        +canExit() bool
        +onMoveLeft/Right/Jump/Crouch/Attack()
        +onStopLeft/Right/Crouch()
        +onClimb()
    }

    class PlayerState {
        <<abstract>>
        #Player& player
        #changePlayerState(newState)
    }

    class PlayerIdleState
    class PlayerRunState
    class PlayerJumpState
    class PlayerFallState
    class PlayerCrouchState
    class PlayerHurtState
    class PlayerDieState
    class PlayerSkillState {
        +execute(ISkill*)
        +isHitboxActive() bool
        +getCurrentSkill() ISkill*
    }
    class PlayerSwimState
    class PlayerClimbState

    IEntityState~Player~ <|-- PlayerState
    PlayerState <|-- PlayerIdleState
    PlayerState <|-- PlayerRunState
    PlayerState <|-- PlayerJumpState
    PlayerState <|-- PlayerFallState
    PlayerState <|-- PlayerCrouchState
    PlayerState <|-- PlayerHurtState
    PlayerState <|-- PlayerDieState
    PlayerState <|-- PlayerSkillState
    PlayerState <|-- PlayerSwimState
    PlayerState <|-- PlayerClimbState
```

---

## 4. Skill System

```mermaid
classDiagram
    direction LR

    class ISkill {
        <<interface>>
        #float manaCost, duration
        #string animationName
        #string nextComboSkillName
        #int attackPower, defensePower
        #Rectangle hitboxConfig
        +execute(Player&)* void
        +getAttackPower() int
        +getHitboxConfig() Rectangle
        +hasNextCombo() bool
    }

    class DashSkill
    class BlockSkill
    class LongAttackSkill
    class Punch1Skill
    class Punch2Skill {
        +nextComboSkillName = "Punch3"
    }
    class Punch3Skill
    class Punch4Skill

    ISkill <|-- DashSkill
    ISkill <|-- BlockSkill
    ISkill <|-- LongAttackSkill
    ISkill <|-- Punch1Skill
    ISkill <|-- Punch2Skill
    ISkill <|-- Punch3Skill
    ISkill <|-- Punch4Skill
```

---

## 5. Command Pattern

```mermaid
classDiagram
    direction TB

    class IPlayerCommand {
        <<interface>>
        +Execute(Player&)* void
    }

    class MoveLeftCommand
    class MoveRightCommand
    class JumpCommand
    class AttackCommand
    class CrouchCommand
    class StopCrouchCommand
    class StopLeftCommand
    class StopRightCommand
    class UseSkillCommand {
        -string skillName
    }
    class InteractCommand
    class ClimbCommand

    IPlayerCommand <|-- MoveLeftCommand
    IPlayerCommand <|-- MoveRightCommand
    IPlayerCommand <|-- JumpCommand
    IPlayerCommand <|-- AttackCommand
    IPlayerCommand <|-- CrouchCommand
    IPlayerCommand <|-- StopCrouchCommand
    IPlayerCommand <|-- StopLeftCommand
    IPlayerCommand <|-- StopRightCommand
    IPlayerCommand <|-- UseSkillCommand
    IPlayerCommand <|-- InteractCommand
    IPlayerCommand <|-- ClimbCommand

    class IGameCommand {
        <<interface>>
        +execute(StateManager&)* void
    }

    class InputHandler {
        -map~int, Binding~ keyBindings_
        +bindKey(key, IPlayerCommand, isPressed)
        +handleInput() vector~IPlayerCommand*~
    }

    InputHandler o-- IPlayerCommand : "stores"
```

---

## 6. Effects System

```mermaid
classDiagram
    direction LR

    class IEffect {
        <<interface>>
        +update(Entity&, dt)* bool
        +refresh()* void
        +getName()* string
    }

    class PoisonEffect {
        -float duration = 5.0f
        -float tickTimer
        +update() : takeDamage(1) per second
        +refresh() : duration = 5s
    }

    class LavaEffect {
        -float duration = 3.0f
        -float tickTimer
        -bool inLava
        +update() : takeDamage(1) per 0.2s(lava) / 0.5s(out)
        +refresh() : duration = 3s
        +setInLava(bool)
    }

    IEffect <|-- PoisonEffect
    IEffect <|-- LavaEffect
```

---

## 7. Game State Hierarchy

**`BaseLevelState`** là lớp cốt lõi nhất đại diện cho một màn chơi đang hoạt động. Nó nắm giữ (composition) toàn bộ hệ thống (TileMap, MapCamera, CombatSystem), các thực thể (Players, Items, Enemies), và hàng đợi (CommandQueue). Nó đóng vai trò là Orchestrator chạy vòng lặp 4 bước (Input -> Process -> Update -> Render).

```mermaid
classDiagram
    direction LR

    class GameState {
        <<abstract>>
        +HandleInput()* void
        +Process()* void
        +Update(dt)* void
        +Render(alpha)* void
        +PushStateCommand(IGameCommand)
        +ConsumeCommands()
    }

    class BaseLevelState {
        #TileMap map
        #MapCamera mapCamera
        #CombatSystem combatSystem
        #Player player1, player2
        #PartyInventory partyInventory
        #InputHandler player1Handler, player2Handler
        #CommandQueue spawnQueue
        #vector~Entity~ activeEntities
        #vector~BaseItem~ activeItems
        #map~string, ItemState~ persistedItemStates
        +HandleInput()
        +Process()
        +Update(dt)
        +Render(alpha)
        -TransitionToLevel(next, dir, gX, gY)
    }

    class IntroState
    class World01State
    class World02State
    class World03State
    class World04State
    class World05State
    class World06State

    GameState <|-- BaseLevelState
    GameState <|-- IntroState
    BaseLevelState <|-- World01State
    BaseLevelState <|-- World02State
    BaseLevelState <|-- World03State
    BaseLevelState <|-- World04State
    BaseLevelState <|-- World05State
    BaseLevelState <|-- World06State
```

---

## 8. Infrastructure — TileMap & Buff

**`TileMap`** là trung tâm nạp và lưu trữ dữ liệu bản đồ. Khi được gọi `LoadLDtkMap`, nó sẽ parse tệp LDtk (.json) để tạo mảng lưới va chạm (`collisionLayer`), các điểm sinh quái/vật phẩm (`entityData_`), và vẽ trước (bake) toàn bộ cảnh vật tĩnh lên một bảng vẽ (`mapCanvas` RenderTexture) để tối ưu hiệu suất Render.

```mermaid
classDiagram
    direction LR

    class IBuffEffect {
        <<interface>>
        +getName()* string
        +getFrameName()* string
        +getDuration()* float
    }

    class SpeedBuff
    class StrengthBuff
    class ShieldBuff
    class JumpBuff
    class InvisiBuff
    class GoldMagnetBuff
    class TimeStopBuff

    IBuffEffect <|-- SpeedBuff
    IBuffEffect <|-- StrengthBuff
    IBuffEffect <|-- ShieldBuff
    IBuffEffect <|-- JumpBuff
    IBuffEffect <|-- InvisiBuff
    IBuffEffect <|-- GoldMagnetBuff
    IBuffEffect <|-- TimeStopBuff

    class TileMap {
        -vector~vector~CollisionType~~ collisionLayer
        -map~int, Texture2D~ tilesetTextures
        -vector~LDtkEntityData~ entityData_
        -RenderTexture2D mapCanvas
        +LoadLDtkMap(path, level) bool
        +GetCollidingTiles(rect) vector~CollisionTile~
        +GetEntityData() vector~LDtkEntityData~
        +GetNeighbour(dir, x, y) string
        +Draw()
    }

    class ItemAtlasRegistry {
        <<Singleton>>
        -map~string, FrameInfo~ frames_
        -map~string, Texture2D~ textures_
        +loadAll(dir)
        +getTexture(name) Texture2D&
        +getFrame(name) Rectangle
    }

    class ItemFactory {
        <<static>>
        +create(id, pos, json) BaseItem*
        +createDynamic(id, pos) BaseItem*
        +createDynamic(id, pos, vel) BaseItem*
    }
```
