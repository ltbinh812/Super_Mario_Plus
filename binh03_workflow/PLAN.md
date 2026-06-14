# ANALYSIS REPORT & IMPLEMENTATION PLAN
# PROJECT: SUPER MARIO CLONE (C++17 / OOP / SDL2)

---

> **Document Type  :** Enterprise System Design Report  
> **Role           :** Enterprise System Architect · Senior Software Engineer · CS Professor  
> **Audience       :** First-year Computer Science Student — Enterprise-grade from Year One  
> **Date           :** 2026-05-30  
> **Version        :** v2.0 — Extended Full Detail  
> **Status         :** 🟡 AWAITING USER APPROVAL — No code has been written yet  

---

## TABLE OF CONTENTS

- [PART I    — ORIGINAL GAME ANALYSIS](#part-i)
- [PART II   — TECHNICAL ANALYSIS & INPUT/OUTPUT](#part-ii)
- [PART III  — DETAILED SYSTEM ARCHITECTURE](#part-iii)
- [PART IV   — GAME LOOP & PER-FRAME LOGIC](#part-iv)
- [PART V    — COMPLETE ENTITY LIST](#part-v)
- [PART VI   — DETAILED WORLD STRUCTURE (8 WORLDS)](#part-vi)
- [PART VII  — RISK MANAGEMENT & EXCEPTION HANDLING](#part-vii)
- [PART VIII — TESTING STRATEGY & EDGE CASES](#part-viii)
- [PART IX   — AGILE ROADMAP (5 PHASES)](#part-ix)
- [PART X    — TECH STACK & DEPENDENCIES](#part-x)

---

# PART I — ORIGINAL GAME ANALYSIS (GAME ANALYSIS REPORT)

> *Mandatory research before any design step. Without understanding the original product, cloning it correctly is impossible.*

---

## 1.1 Game Overview

| Attribute | Information |
|---|---|
| **Original Title** | Super Mario Bros. (スーパーマリオブラザーズ) |
| **Developer** | Nintendo R&D4 — Shigeru Miyamoto & Takashi Tezuka |
| **Original Platform** | NES (Famicom) — 1985 |
| **Original Resolution** | 256 × 240 pixels |
| **Frame Rate** | 60 FPS (NTSC) |
| **World Count** | 8 Worlds × 4 Levels = 32 stages |
| **Tile Size** | 16 × 16 pixels |
| **Viewport** | 16 tiles × 15 tiles visible at once |

---

## 1.2 Movement Mechanics

### 1.2.1 Horizontal Movement

```
← / →   : Move left/right
B/Shift  : Hold to run (increase speed)

Speed values:
  walk_speed      = 1.5  NES units/frame  (~94 px/s at 60FPS)
  run_speed       = 2.5  NES units/frame  (~156 px/s at 60FPS)
  max_speed_walk  = 1.5
  max_speed_run   = 2.5

Acceleration / Deceleration:
  acceleration_walk  = 0.098  /frame  (accelerate when walking)
  acceleration_run   = 0.198  /frame  (accelerate when running)
  deceleration       = 0.5    /frame  (release key → slide to stop)
  skid_deceleration  = 0.8    /frame  (reverse direction at high speed)

Skid Effect:
  Condition : velocity.x > 1.0 AND changing direction
  Effect    : Show "skid" sprite, emit dust particle
  After     : velocity.x sign reverses, continue accelerating in new direction
```

### 1.2.2 Jump — Most Complex Mechanic

```
Basic Jump:
  jump_initial_velocity = -8.0 NES units/frame  (upward = negative)
  jump_hold_gravity     = 0.25 /frame²  (gravity reduced while holding jump)
  normal_gravity        = 0.5  /frame²
  max_fall_speed        = 6.0  NES units/frame

Variable Jump Height:
  - Tap and release quickly  → 6 frames held  → low jump  (~3 tiles)
  - Hold until auto-release  → 12 frames held → high jump (~5 tiles)
  - Mechanic: During the first 12 frames AFTER jump begins,
    if holding jump button → gravity = 0.25 (lighter)
    if jump released early → gravity = 0.5  (heavier → fall faster)

Horizontal speed affects jump arc:
  Standing still jump   → tall, narrow arc
  Walking jump          → medium arc
  Full sprint jump      → Long Jump: travel ~6-7 tiles horizontally

Coyote Time (Grace window after platform edge):
  coyote_time = 6 frames (~0.1 seconds)
  Logic: If Mario JUST left the ground (≤ 6 frames ago) and presses jump
         → still performs a normal jump (not a double jump)
  Why: Player presses jump "slightly late" → game still registers it
       → game feels fair, no unfair deaths

Jump Buffer (Buffered jump input):
  jump_buffer_time = 6 frames (~0.1 seconds)
  Logic: If Mario presses jump while still airborne
         (≤ 6 frames from ground) → store jump command in buffer
         When Mario lands → execute jump immediately
  Why: Eliminates the "pressed jump but nothing happened" input lag feeling
```

### 1.2.3 Airborne Physics States

| State | Condition | Gravity | Notes |
|---|---|---|---|
| `GROUNDED` | `onGround == true` | N/A | Running, standing, skidding |
| `RISING` | `velocity.y < 0` and holding jump | 0.25/frame | Variable jump |
| `RISING_RELEASE` | `velocity.y < 0` and jump released | 0.5/frame | Falls faster |
| `APEX` | `|velocity.y| < 0.5` | 0.5/frame | Peak of jump arc |
| `FALLING` | `velocity.y > 0` | 0.5/frame | Normal falling |
| `FAST_FALLING` | `velocity.y >= max_fall_speed` | 0 (already at max) | Terminal velocity |

---

## 1.3 Physics System

### 1.3.1 AABB Collision Detection

```
AABB = Axis-Aligned Bounding Box
Each entity has a rectangular hitbox (x, y, w, h)

Mario's hitbox is smaller than sprite for "breathing room":
  Small Mario Sprite : 16 × 16 px
  Small Mario Hitbox : 12 × 14 px  (2px inset per horizontal side, 2px top)

  Super Mario Sprite : 16 × 32 px
  Super Mario Hitbox : 12 × 28 px

AABB Overlap check:
  bool overlap(Rect a, Rect b):
    return (a.x < b.x + b.w) && (a.x + a.w > b.x)
        && (a.y < b.y + b.h) && (a.y + a.h > b.y)

Collision Side Detection (MTV — Minimum Translation Vector):
  overlapX = min(a.right - b.left,  b.right - a.left)
  overlapY = min(a.bottom - b.top,  b.bottom - a.top)

  if overlapX < overlapY:
      → Resolve along X axis (left/right collision)
      → Push entity left or right by overlapX px
  else:
      → Resolve along Y axis (top/bottom collision)
      → Push entity up or down by overlapY px

Corner Correction:
  If Mario jumps into a tile corner and X overlap is only 1-3px:
  → Auto-nudge horizontally 1-3px to avoid being blocked
  → Player feels Mario "slip through" the corner
```

### 1.3.2 Tile Collision Priority

```
Each frame, process collisions in this order:
  1. Mario vs Tiles (solid ground)     — highest priority
  2. Mario vs Enemy                    — determine kill/hurt
  3. Mario vs Item                     — pickup
  4. Projectile vs Enemy               — fireball hit
  5. Projectile vs Tile                — fireball bounce/destroy
  6. Enemy vs Tile                     — enemy reverses direction
  7. Shell vs Enemy                    — shell sliding
```

### 1.3.3 Platform Types

| Tile Type | Block Top | Block Bottom | Block Sides | Notes |
|---|---|---|---|---|
| `SOLID` | ✅ | ✅ | ✅ | Ground, wall |
| `ONE_WAY` | ✅ | ❌ | ❌ | Cloud platform |
| `BRICK` | ✅ | ✅ | ✅ | Breaks when Super Mario hits from below |
| `QUESTION` | ✅ | ✅ | ✅ | Releases item when hit from below |
| `EMPTY_BLOCK` | ✅ | ✅ | ✅ | Used-up block |
| `PIPE` | ✅ | ✅ | ✅ | Pipe, Mario can enter |
| `DEATH_ZONE` | ❌ | ❌ | ❌ | Fall in → die |

---

## 1.4 Mario State Machine (FSM)

```
┌─────────────────────────────────────────────────────────────┐
│                     MARIO FSM                                │
│                                                             │
│  [SMALL] ──Mushroom──→ [SUPER] ──FireFlower──→ [FIRE]       │
│     ↑                      │                      │         │
│     └──────── Hit ─────────┘                      │         │
│     ↑                                             │         │
│     └──────────────── Hit ────────────────────────┘         │
│                                                             │
│  [ANY STATE] ──Star──→ [INVINCIBLE 10s] ──timeout──→ restore│
│                                                             │
│  [SMALL + Hit] ──→ [DYING] ──→ [DEAD] ──→ Lives--          │
│                                                             │
│  [DEAD + Lives>0] ──→ [RESPAWN at checkpoint]               │
│  [DEAD + Lives=0] ──→ [GAME_OVER]                           │
└─────────────────────────────────────────────────────────────┘
```

| State | Size | Ability | When Hit |
|---|---|---|---|
| `SMALL` | 16×16 | Walk, jump, stomp | → `DYING` |
| `SUPER` | 16×32 | + Break Bricks | → `SMALL` (invulnerable 2s) |
| `FIRE` | 16×32 | + Shoot Fireballs | → `SUPER` (invulnerable 2s) |
| `INVINCIBLE` | 16×32 | + Kill enemies on contact | Immune |
| `DYING` | 16×16 | None | N/A |

---

## 1.5 Score & Lives System

### 1.5.1 Score Table

| Action | Points | Notes |
|---|---|---|
| Stomp Goomba | 100 | |
| Stomp Koopa | 100 | |
| Fireball kills enemy | 200 | |
| Shell kills enemy #1 | 200 | Combo scales up |
| Shell kills enemy #2 | 400 | |
| Shell kills enemy #3 | 800 | |
| Shell kills enemy #4+ | 1000+ | Capped at 8000 |
| Consecutive stomp combo | 100→200→400→800→1000→2000→4000→8000→1UP | |
| Collect coin | 200 | |
| Hit block for coin | 200 | |
| Pick up Mushroom | 1000 | |
| Pick up FireFlower | 1000 | |
| Pick up Star | 1000 | |
| Kill enemy while Starman | 200→400→800→... | Combo |
| Flagpole level 1 (lowest) | 100 | |
| Flagpole level 2 | 400 | |
| Flagpole level 3 | 800 | |
| Flagpole level 4 | 2000 | |
| Flagpole level 5 (highest) | 5000 | |
| Flagpole absolute top | 5000 + 3 Fireworks | |
| Time Bonus | remaining × 50 points | When clearing a level |
| 100 Coins | +1 Life | Coin count resets to 0 |

### 1.5.2 Timer & Lives

```
Timer:
  Time per level : 200 – 400 seconds (varies by level)
  Warning        : ≤ 100s remaining → BGM tempo increases
  Time out       : Mario dies immediately (even without enemies)

Lives:
  Starting lives : 3
  +1 life        : 100 coins | 1-Up Mushroom | special stomp combo
  Game Over      : Lives = 0 → return to start of current World
```

---

## 1.6 Audio System

```
BGM (Background Music):
  overworld_theme     : World 1,3,5,6 (plains, clouds, snow)
  underground_theme   : World 1-2, 2-2, 4-2, 7-2 (underground)
  underwater_theme    : World 2-2, 3-1, 3-3, 6-3 (underwater)
  castle_theme        : X-4 (castle)
  starman_theme       : During Star (overrides BGM, resumes after)
  hurry_up_theme      : ≤ 100s remaining (faster version of current BGM)
  world_clear_fanfare : After grabbing the flag
  castle_clear_fanfare: After defeating the Boss
  game_over_theme     : Game Over screen
  title_screen_theme  : Main menu

SFX (Sound Effects):
  jump.wav            : Every jump
  stomp.wav           : Stomping an enemy
  kick.wav            : Kicking a shell
  coin.wav            : Collecting a coin (short, high pitch)
  powerup_appear.wav  : Item appears from block
  powerup_pickup.wav  : Picking up an item
  powerdown.wav       : Getting hit, losing power state
  death.wav           : Mario falls (separate from BGM)
  fireball.wav        : Shooting a fireball
  fireball_hit.wav    : Fireball hits enemy/wall
  flagpole.wav        : Grabbing the flagpole
  bowser_fire.wav     : Bowser shoots fire
  bowser_fall.wav     : Bowser falls into lava
  1up.wav             : +1 life
  brick_smash.wav     : Breaking a brick
  block_hit.wav       : Hitting an already-used block
  pipe.wav            : Mario enters pipe
  pause.wav           : Pause game
  levelclear.wav      : Level clear
  gameover.wav        : Game over
```

---

# PART II — TECHNICAL ANALYSIS & INPUT/OUTPUT

---

## 2.1 Input / Output Specification

```
INPUT SOURCES:
  ┌─ Keyboard ──────────────────────────────────────────────┐
  │  LEFT / A       : Move Mario left                       │
  │  RIGHT / D      : Move Mario right                      │
  │  SPACE / W / UP : Jump (hold = jump higher)             │
  │  SHIFT / Z      : Run fast + (FireMario) Shoot fireball │
  │  DOWN / S       : Crouch (Super/Fire) | Enter pipe      │
  │  ESC            : Pause / Return to menu                │
  │  ENTER          : Confirm, Start game                   │
  │  F1             : Toggle debug mode (hitbox, FPS)       │
  └─────────────────────────────────────────────────────────┘
  ┌─ File Input ────────────────────────────────────────────┐
  │  assets/maps/world_X_Y.json  : Tilemap data            │
  │  assets/sprites/*.png        : Sprite sheets           │
  │  assets/sounds/*.wav         : Sound effects           │
  │  assets/music/*.wav          : Background music        │
  │  saves/save.json             : Save state              │
  │  saves/highscores.dat        : Highscore list          │
  └─────────────────────────────────────────────────────────┘

OUTPUT TARGETS:
  ┌─ Display ───────────────────────────────────────────────┐
  │  Window  : 800×600 px (scaled from 256×240 NES)         │
  │  HUD     : Score, Coins, World(X-Y), Timer, Lives       │
  │  Debug   : Hitbox overlay, FPS counter, entity count   │
  └─────────────────────────────────────────────────────────┘
  ┌─ Audio ─────────────────────────────────────────────────┐
  │  BGM channel  : 1 track, loop                          │
  │  SFX channels : 8 concurrent channels (SDL2_mixer)     │
  └─────────────────────────────────────────────────────────┘
  ┌─ File Output ───────────────────────────────────────────┐
  │  saves/save.json    : Current world/level/score/lives  │
  │  saves/highscores.dat: Top 10 scores                   │
  │  logs/game.log      : Runtime error log                │
  └─────────────────────────────────────────────────────────┘
```

---

## 2.2 Complete Design Patterns

| # | Pattern | Applied At | Problem Solved |
|---|---|---|---|
| 1 | **Game Loop** (Fixed Timestep) | `GameLoop::run()` | Consistent physics at any FPS |
| 2 | **Finite State Machine** | `MarioFSM`, `GameStateMachine`, `EnemyAI` | Clear state management, no messy if-else chains |
| 3 | **Factory Method** | `EntityFactory::create()` | Flexibly create entities from map data |
| 4 | **Observer / Event Bus** | Global `EventBus` | Score, Audio, Particle decoupled from each other |
| 5 | **Singleton** | `AssetManager`, `AudioManager`, `Logger`, `EventBus` | Shared resources, initialized once |
| 6 | **Strategy** | `AIBehavior` per enemy type | Add new enemies without modifying GameLoop |
| 7 | **Object Pool** | `Pool<Fireball>`, `Pool<Particle>`, `Pool<Coin>` | Avoid new/delete inside gameplay loop |
| 8 | **Flyweight** | `TileRenderer` (shared texture) | 10,000 tiles but only 1 texture loaded per tileset |
| 9 | **Template Method** | `Entity::update()`, `Enemy::update()` | Common framework, subclasses fill in details |
| 10 | **Command** | `InputHandler` | Easy key rebinding, replay system later |
| 11 | **Composite** | `SceneManager` (state stack) | Push/pop states like a stack |
| 12 | **RAII** | `AssetManager` destructor, `unique_ptr` | Memory leaks are impossible when coded correctly |

---

## 2.3 SOLID Principles Applied

```
S — Single Responsibility Principle
    ✅ PhysicsSystem  : only updates velocity/position
    ✅ CollisionSystem: only detects and resolves collisions
    ✅ RenderSystem   : only draws entities to screen
    ✅ AudioSystem    : only plays audio in response to events
    ❌ Common violation (avoid): 1 class that updates, renders, and handles input

O — Open/Closed Principle
    Adding a new enemy: create subclass of Enemy, override AI → DO NOT modify GameLoop
    ✅ Goomba extends Enemy → override onUpdate(), onStomped()
    ✅ KoopaTroopa extends Enemy → override onUpdate(), onStomped(), onShell()

L — Liskov Substitution Principle
    Entity* ptr = new Mario / new Goomba / new Fireball
    → Every function accepting Entity* works correctly with every subtype

I — Interface Segregation Principle
    IRenderable  { virtual void render(Renderer&) = 0; }
    IUpdatable   { virtual void update(float dt) = 0; }
    ICollidable  { virtual Rect getHitbox() = 0;
                   virtual void onCollide(Entity*, Side) = 0; }
    Mario : IRenderable, IUpdatable, ICollidable
    HUD   : IRenderable, IUpdatable (does not need ICollidable)

D — Dependency Inversion Principle
    GameLoop depends on IGameState (interface)
    Does NOT depend directly on PlayState or MenuState
    → Can add PauseState, CinematicState without modifying GameLoop
```

---

# PART III — DETAILED SYSTEM ARCHITECTURE

---

## 3.1 Layered Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                   PRESENTATION LAYER                         │
│         SDL2_Window · SDL2_Renderer · SDL2_AudioDevice       │
│                    (Hardware abstraction)                    │
├──────────────────────────┬──────────────────────────────────┤
│     APPLICATION LAYER    │          UI LAYER                 │
│  GameLoop · GameManager  │   HUD · Menu · PauseScreen       │
│  SceneManager (State Stack│   SplashScreen · GameOverScreen  │
├──────────────────────────┴──────────────────────────────────┤
│                     DOMAIN LAYER                             │
│  ┌─────────────┬───────────────┬───────────────────────┐   │
│  │  ENTITY     │    WORLD      │      SYSTEMS           │   │
│  │  Mario      │  TileMap      │  PhysicsSystem         │   │
│  │  Enemy      │  Camera       │  CollisionSystem       │   │
│  │  Item       │  LevelLoader  │  RenderSystem          │   │
│  │  Projectile │  EntityFactory│  InputSystem           │   │
│  │  Particle   │               │  AudioSystem           │   │
│  └─────────────┴───────────────┴───────────────────────┘   │
├─────────────────────────────────────────────────────────────┤
│                  INFRASTRUCTURE LAYER                        │
│  AssetManager · AudioManager · Logger · EventBus            │
│  ObjectPool · SpatialHash · FileIO · Constants              │
└─────────────────────────────────────────────────────────────┘
```

## 3.2 System Relationship Diagram

```
InputSystem
    │ (InputState)
    ▼
GameLoop ──────────────────────────────────────────────────┐
    │                                                       │
    ├──→ PhysicsSystem.update(entities, dt)                 │
    │       │ updates velocity, position, grounded state    │
    │       ▼                                               │
    ├──→ CollisionSystem.update(entities, tilemap)          │
    │       │ detects overlap → resolve → publish Event     │
    │       ▼                                               │
    ├──→ EntitySystem.update(entities, dt)                  │
    │       │ enemy AI, Mario FSM, animation, timers        │
    │       ▼                                               │
    ├──→ EventBus.dispatch(pendingEvents)                   │
    │       │ Score, Audio, Particle, Lives...              │
    │       ▼                                               │
    └──→ RenderSystem.render(entities, camera, hud)
            │ Cull off-screen → sort by layer → draw
            ▼
         SDL2_Renderer.present()
```

## 3.3 Full Project Directory Tree

```
super_mario/
│
├── CMakeLists.txt                    # Root build configuration
├── README.md                         # Build and play instructions
├── PLAN.md                           # This document
├── .cursorrules                      # AI coding rules
├── Doxyfile                          # API documentation config
│
├── assets/
│   ├── sprites/
│   │   ├── mario_small.png           # 6 frames: stand, walk×3, jump, skid
│   │   ├── mario_super.png           # 8 frames: stand, walk×3, jump, skid, crouch, climb
│   │   ├── mario_fire.png            # Same as super + shoot frame
│   │   ├── mario_dead.png            # 1 frame death pose
│   │   ├── enemies.png               # All enemy sprite sheet
│   │   ├── items.png                 # Mushroom, Flower, Star, 1Up, Coin
│   │   ├── tileset_overworld.png     # World 1,3,5,6
│   │   ├── tileset_underground.png   # World 2 underground
│   │   ├── tileset_castle.png        # X-4 Castle
│   │   ├── tileset_water.png         # Underwater level
│   │   ├── hud_numbers.png           # Pixel font 0-9
│   │   ├── hud_icons.png             # Lives icon, coin icon
│   │   └── particles.png             # Brick shards, stars, smoke
│   │
│   ├── sounds/
│   │   ├── jump.wav
│   │   ├── stomp.wav
│   │   ├── kick.wav
│   │   ├── coin.wav
│   │   ├── powerup_appear.wav
│   │   ├── powerup_pickup.wav
│   │   ├── powerdown.wav
│   │   ├── death.wav
│   │   ├── fireball.wav
│   │   ├── fireball_hit.wav
│   │   ├── flagpole.wav
│   │   ├── bowser_fire.wav
│   │   ├── bowser_fall.wav
│   │   ├── 1up.wav
│   │   ├── brick_smash.wav
│   │   ├── block_hit.wav
│   │   ├── pipe.wav
│   │   └── pause.wav
│   │
│   ├── music/
│   │   ├── overworld.wav
│   │   ├── overworld_hurry.wav
│   │   ├── underground.wav
│   │   ├── underground_hurry.wav
│   │   ├── underwater.wav
│   │   ├── underwater_hurry.wav
│   │   ├── castle.wav
│   │   ├── castle_hurry.wav
│   │   ├── starman.wav
│   │   ├── world_clear.wav
│   │   ├── castle_clear.wav
│   │   ├── game_over.wav
│   │   └── title.wav
│   │
│   └── maps/
│       ├── world_1_1.json  through  world_8_4.json  (32 files total)
│
├── include/
│   ├── core/
│   │   ├── Game.h                    # Init, cleanup, system orchestration
│   │   ├── GameLoop.h                # Fixed-timestep main loop
│   │   └── Window.h                  # SDL2 Window + Renderer wrapper
│   │
│   ├── states/
│   │   ├── IGameState.h              # Interface: onEnter, onExit, update, render
│   │   ├── SplashState.h             # Nintendo logo splash screen
│   │   ├── TitleState.h              # Title screen + "Press Start"
│   │   ├── MenuState.h               # World select (1-Player / 2-Player)
│   │   ├── PlayState.h               # Main gameplay state
│   │   ├── PauseState.h              # Overlay pause (does not destroy PlayState)
│   │   ├── TransitionState.h         # Scene transition between levels
│   │   └── GameOverState.h           # Game over screen
│   │
│   ├── entities/
│   │   ├── Entity.h                  # Abstract base: id, pos, vel, active, update, render
│   │   │
│   │   ├── mario/
│   │   │   ├── Mario.h               # Player entity
│   │   │   ├── MarioFSM.h            # State machine: SMALL/SUPER/FIRE/INVINCIBLE/DYING
│   │   │   └── MarioInput.h          # Input → Action mapping for Mario
│   │   │
│   │   ├── enemies/
│   │   │   ├── Enemy.h               # Abstract: direction, speed, onStomped, onFireball
│   │   │   ├── Goomba.h
│   │   │   ├── KoopaTroopa.h         # Shell mechanic
│   │   │   ├── KoopaShell.h          # Sliding shell entity
│   │   │   ├── PiranhaPlant.h        # Lives inside pipe
│   │   │   ├── HammerBro.h           # Throws hammers, jumps on platforms
│   │   │   ├── Lakitu.h              # Flies on cloud, throws Spinies
│   │   │   ├── Spiny.h               # Cannot be stomped
│   │   │   ├── BulletBill.h          # Horizontal projectile enemy
│   │   │   ├── BulletBillLauncher.h  # Spawns BulletBills
│   │   │   ├── CheepCheep.h          # Fish: underwater + leaping
│   │   │   ├── Blooper.h             # Squid underwater enemy
│   │   │   ├── FireBar.h             # Rotating fire bar in castle
│   │   │   ├── Bowser.h              # Final boss
│   │   │   └── FakeBowser.h          # Goomba/Koopa disguised as Bowser (Worlds 1-7)
│   │   │
│   │   ├── items/
│   │   │   ├── Item.h                # Abstract: onPickup(Mario&)
│   │   │   ├── Mushroom.h            # Small→Super
│   │   │   ├── FireFlower.h          # Super→Fire
│   │   │   ├── StarPower.h           # Invincible 10s
│   │   │   └── OneUpMushroom.h       # +1 Life
│   │   │
│   │   ├── tiles/
│   │   │   ├── Tile.h                # Data: TileType, hasItem, itemType, isUsed
│   │   │   ├── TileType.h            # Enum: AIR, SOLID, BRICK, QUESTION, PIPE...
│   │   │   ├── QuestionBlock.h       # Block hit logic, spawn item/coin
│   │   │   └── BrickBlock.h          # Break logic when Super Mario hits
│   │   │
│   │   └── projectiles/
│   │       ├── Projectile.h          # Abstract: damage, velocity
│   │       ├── Fireball.h            # Bounces on ground, max 2 at once
│   │       ├── Hammer.h              # Thrown by Hammer Bro, arc trajectory
│   │       └── BowserFire.h          # Bowser's fireball (larger, no bounce)
│   │
│   ├── components/
│   │   ├── PhysicsComponent.h        # velocity, grounded, gravity_scale
│   │   ├── RenderComponent.h         # texture*, src_rect, dst_rect, flip, layer
│   │   ├── ColliderComponent.h       # hitbox Rect, isTrigger, layer mask
│   │   └── AnimationComponent.h      # animations map, current, frame, timer, loop
│   │
│   ├── systems/
│   │   ├── PhysicsSystem.h           # Updates velocity → position for all entities
│   │   ├── CollisionSystem.h         # AABB check, resolve, publish Events
│   │   ├── RenderSystem.h            # Frustum cull, sort by layer, SDL2_RenderCopy
│   │   ├── AudioSystem.h             # Listens to Events → plays SFX/BGM
│   │   ├── InputSystem.h             # Polls SDL_Event → InputState struct
│   │   ├── ParticleSystem.h          # Updates/renders Particle objects
│   │   └── ScoreSystem.h             # Listens to Events → adds score, checks 100 coins
│   │
│   ├── world/
│   │   ├── TileMap.h                 # 2D grid of tiles, render, getTileAt(x,y)
│   │   ├── Camera.h                  # position, follows Mario, clamped to map boundary
│   │   ├── LevelLoader.h             # JSON → TileMap + EntitySpawnList
│   │   └── EntityFactory.h           # EntityType enum → new Entity*(config)
│   │
│   ├── ui/
│   │   ├── HUD.h                     # Score, Coin, World, Timer, Lives rendering
│   │   ├── Menu.h                    # Menu item list, selection, callbacks
│   │   └── NumberRenderer.h          # Renders numbers using pixel font sprite
│   │
│   └── utils/
│       ├── AssetManager.h            # Singleton: load/cache SDL_Texture, Mix_Chunk
│       ├── AudioManager.h            # Singleton: play BGM, play SFX, pause, resume
│       ├── EventBus.h                # Singleton: subscribe<T>, publish<T>
│       ├── Logger.h                  # Singleton: log levels, file output
│       ├── ObjectPool.h              # Template<T>: allocate, release, reset
│       ├── SpatialHash.h             # Divide world into grid, query neighbors
│       ├── Vector2D.h                # struct { float x,y; operators; length; normalize }
│       ├── Rect.h                    # struct { float x,y,w,h; overlap; contains; }
│       ├── MathUtils.h               # clamp, lerp, sign, abs
│       └── Constants.h               # GRAVITY, TILE_SIZE, SCREEN_W, SCREEN_H, FPS...
│
├── src/                              # Mirrors include/ structure, .cpp files
│
├── tests/
│   ├── CMakeLists.txt
│   ├── test_physics.cpp
│   ├── test_collision.cpp
│   ├── test_mario_fsm.cpp
│   ├── test_animation.cpp
│   ├── test_tilemap_loader.cpp
│   ├── test_asset_manager.cpp
│   ├── test_object_pool.cpp
│   ├── test_event_bus.cpp
│   ├── test_enemy_ai.cpp
│   └── test_spatial_hash.cpp
│
├── saves/
│   ├── save.json                     # (auto-generated)
│   └── highscores.dat                # (auto-generated)
│
└── logs/
    └── game.log                      # (auto-generated)
```

---

## 3.4 Detailed Class Hierarchy

```
Entity (abstract)
│   # id       : int
│   # position : Vector2D
│   # velocity : Vector2D
│   # active   : bool
│   # layer    : int  (0=bg, 1=world, 2=entity, 3=ui)
│   ──────────────────────────────────────────
│   + update(dt: float): void   [pure virtual]
│   + render(renderer: Renderer&): void [pure virtual]
│   + getHitbox(): Rect         [pure virtual]
│   + onCollide(other: Entity*, side: Side): void [virtual, empty]
│   + isActive(): bool
│   + setActive(bool)
│
├── Mario
│   + fsm            : MarioFSM
│   + lives          : int
│   + coins          : int
│   + score          : int
│   + isRunning      : bool
│   + facingRight    : bool
│   - coyoteTimer    : float
│   - jumpBufferTimer: float
│   - jumpHoldTimer  : float
│   - isHoldingJump  : bool
│   - invulTimer     : float   (2s after being hit)
│   - fireball1Active: bool
│   - fireball2Active: bool
│   ──────────────────────────────────────────
│   + update(dt)
│   + render(renderer)
│   + getHitbox(): Rect
│   + onCollide(other, side)
│   - handleMovement(input, dt)
│   - handleJump(input, dt)
│   - applyPhysics(dt)
│   - shoot()
│   - takeDamage()
│   - die()
│   - collect(Item*)
│
├── Enemy (abstract)
│   # direction  : int  (-1=left, +1=right)
│   # moveSpeed  : float
│   # isDead     : bool
│   # deathTimer : float   (delay before removal)
│   # scoreValue : int
│   ──────────────────────────────────────────
│   + update(dt)   [template method — calls onUpdateAI]
│   + onCollide(other, side)
│   # onUpdateAI(dt): void [pure virtual — unique AI per enemy]
│   # onStomped()  : void [virtual]
│   # onFireball() : void [virtual]
│   # onShell()    : void [virtual]
│   # onStar()     : void [virtual]
│
│   ├── Goomba
│   │   # onUpdateAI(dt): walk horizontally, reverse on wall contact
│   │   # onStomped()   : play flatten anim, deathTimer=0.3s, score+100
│   │
│   ├── KoopaTroopa
│   │   + shellState : ShellState {WALKING, IN_SHELL, SLIDING, REVIVING}
│   │   + reviveTimer: float
│   │   # onUpdateAI(dt): walk, check ShellState
│   │   # onStomped()   : → IN_SHELL
│   │   + kickShell()   : → SLIDING
│   │
│   ├── KoopaShell (separate entity when sliding)
│   │   + speed : float (faster than KoopaTroopa)
│   │   [Shell can kill multiple enemies, bounces back and can kill Mario]
│   │
│   ├── PiranhaPlant
│   │   + pipeCenter    : Vector2D
│   │   + moveDir       : int {UP=1, DOWN=-1}
│   │   + moveTimer     : float
│   │   + waitTimer     : float
│   │   + hiddenIfClose : bool  (hidden if Mario is near pipe opening)
│   │   # onUpdateAI(dt): move up/down on a cycle
│   │   # onStomped()   : does nothing (cannot be stomped)
│   │
│   ├── HammerBro
│   │   + throwTimer : float
│   │   + jumpTimer  : float
│   │   + hammers[]  : ObjectPool<Hammer>
│   │   # onUpdateAI(dt): jump on platforms, throw hammers periodically
│   │
│   ├── Lakitu
│   │   + altitude  : float  (flies at fixed altitude)
│   │   + throwTimer: float
│   │   + spinies[] : ObjectPool<Spiny>
│   │   # onUpdateAI(dt): fly horizontally, throw Spinies downward
│   │
│   ├── Spiny
│   │   # onUpdateAI(dt): walk horizontally like Goomba
│   │   # onStomped()   : Mario takes damage (Spiny has spikes, cannot be stomped)
│   │
│   ├── BulletBill
│   │   # onUpdateAI(dt): fly straight horizontally toward Mario
│   │   # onStomped()   : dies normally
│   │
│   ├── CheepCheep
│   │   + swimType : {STRAIGHT, LEAPING}
│   │   # onUpdateAI(dt): swim horizontal (straight) or jump upward (leaping)
│   │
│   ├── Blooper (squid)
│   │   # onUpdateAI(dt): swim up → fall down → repeat, chase Mario on X axis
│   │
│   ├── FireBar
│   │   + centerTile : Vector2D
│   │   + angle      : float
│   │   + angularVel : float
│   │   + segments   : int  (5-8 fire orbs per bar)
│   │   [Each fire orb is a separate hitbox]
│   │
│   ├── Bowser
│   │   + fireTimer  : float
│   │   + hammerTimer: float
│   │   + hp         : int  (only real Bowser in W8-4 has hp > 0)
│   │   # onUpdateAI(dt): walk horizontal, shoot fire, throw hammers
│   │   # onFireball() : isReal ? hp-- : die()
│   │
│   └── FakeBowser (extends Bowser)
│       [Worlds 1-4 → 7-4: actually a Goomba/Koopa in disguise]
│       # onFireball() : die() immediately (1 Fireball)
│
├── Item (abstract)
│   + isMoving : bool
│   + moveDir  : int
│   ──────────────────────────────────────────
│   # onPickup(mario: Mario&): void [pure virtual]
│   + update(dt)  [moves, falls to ground]
│   + onCollide(other, side)
│
│   ├── Mushroom
│   │   # onPickup: mario.fsm.transition(SUPER), score+=1000
│   │
│   ├── FireFlower
│   │   [Stationary, does not move]
│   │   # onPickup: mario.fsm.transition(FIRE), score+=1000
│   │
│   ├── StarPower
│   │   + bounceVelocity: float
│   │   [Bounces regularly, moves toward Mario's direction]
│   │   # onPickup: mario.fsm.transition(INVINCIBLE), score+=1000
│   │
│   └── OneUpMushroom
│       # onPickup: mario.lives++, play 1up.wav, score+=0
│
├── Projectile (abstract)
│   + damage     : int
│   + maxBounce  : int
│   + bounceCount: int
│   ──────────────────────────────────────────
│   # onHitEnemy(Enemy*): void [virtual]
│   # onHitTile(): void [virtual]
│
│   ├── Fireball
│   │   [Bounces on Y when hitting floor tile, travels straight on X]
│   │   [Destroyed on hitting vertical wall or bounceCount >= 3]
│   │   # onHitEnemy: enemy.onFireball(), score+=200
│   │
│   ├── Hammer
│   │   [Arc trajectory: initial upward velocity + gravity]
│   │   [Destroyed after N seconds or on hitting the ground]
│   │
│   └── BowserFire (larger, no bounce)
│
└── Particle (visual only, not a full Entity)
    + lifetime : float
    + velocity : Vector2D
    + sprite   : SpriteFrame
    [Uses ObjectPool<Particle>, no collision]
    [Types: COIN_SPARKLE, BRICK_SHARD, SCORE_POPUP, DUST, DEATH_STAR]
```

---

## 3.5 Event Bus — Complete Event List

```cpp
// All game events (using std::variant or enum + payload)

enum class EventType {
    // Mario events
    MARIO_JUMPED,           // payload: velocity
    MARIO_LANDED,           // payload: position
    MARIO_DIED,             // payload: lives remaining
    MARIO_STATE_CHANGED,    // payload: old_state, new_state
    MARIO_COIN_COLLECTED,   // payload: position, total_coins
    MARIO_RESPAWNED,        // payload: position
    MARIO_GOT_HIT,          // payload: position

    // Enemy events
    ENEMY_KILLED,           // payload: enemy_type, position, score
    ENEMY_STOMPED,          // payload: enemy_id, position
    ENEMY_SHELL_KICKED,     // payload: shell_id, direction

    // Item events
    ITEM_SPAWNED,           // payload: item_type, position
    ITEM_COLLECTED,         // payload: item_type, mario_state_after

    // Block events
    BLOCK_HIT,              // payload: block_pos, block_type, item_inside
    BLOCK_BROKEN,           // payload: block_pos
    COIN_SPAWNED,           // payload: position

    // Projectile events
    FIREBALL_FIRED,         // payload: position, direction
    FIREBALL_HIT_ENEMY,     // payload: enemy_id
    FIREBALL_DESTROYED,     // payload: position

    // Level events
    LEVEL_START,            // payload: world, level
    LEVEL_COMPLETE,         // payload: time_remaining, score
    TIMER_WARNING,          // payload: time_remaining (when <= 100s)
    GAME_OVER,              // (no payload)
    WORLD_CLEAR,            // payload: world number

    // Score events
    SCORE_ADDED,            // payload: amount, position (shows popup)
    LIVES_CHANGED,          // payload: new_lives
    COIN_COUNT_CHANGED,     // payload: new_count
};

// Example flow when Mario stomps a Goomba:
// CollisionSystem publishes ENEMY_STOMPED {goomba_id, pos}
// → Goomba.onStomped() → switch to death animation
// → ScoreSystem listens → score += 100, publishes SCORE_ADDED {100, pos}
// → AudioSystem listens → plays stomp.wav
// → ParticleSystem listens SCORE_ADDED → spawns "100" popup particle
// → HUD listens SCORE_ADDED → updates score display
```

---

# PART IV — GAME LOOP & PER-FRAME LOGIC

---

## 4.1 Fixed Timestep Game Loop — Detailed Explanation

```
WHY Fixed Timestep?
─────────────────────────────────────────────────────────
If using Variable Timestep (dt = real frame time):
  Fast machine (144 FPS): dt = 0.007s → smooth physics
  Slow machine (15 FPS) : dt = 0.067s → large dt → Mario could
                          tunnel through thin tiles in 1 frame!
  Lag spike 500ms       : Mario "teleports" through walls

Fixed Timestep solution:
  Physics ALWAYS runs at 60Hz (dt = 1/60 = 0.0167s)
  Whether machine renders at 30 or 144 FPS, Mario jumps the same height
  Lag spikes: Physics "catches up" by running multiple small update steps
```

```
FIXED TIMESTEP PSEUDOCODE:
─────────────────────────────────────────────────────────
const float FIXED_DT = 1.0f / 60.0f;   // 16.67ms
const float MAX_FRAME_TIME = 0.25f;     // Prevent "death spiral"
float accumulator = 0.0f;
uint64_t prevTime = SDL_GetPerformanceCounter();

while (game.isRunning()) {
    // 1. Calculate real frame time
    uint64_t currTime = SDL_GetPerformanceCounter();
    float frameTime = (currTime - prevTime) / (float)SDL_GetPerformanceFrequency();
    prevTime = currTime;

    // 2. Clamp to prevent "spiral of death" on slow machines
    frameTime = std::min(frameTime, MAX_FRAME_TIME);
    accumulator += frameTime;

    // 3. Process input (once per real frame, independent of physics rate)
    inputSystem.pollEvents();
    if (inputSystem.isQuit()) game.stop();

    // 4. Physics update — runs at FIXED 60Hz rate
    while (accumulator >= FIXED_DT) {
        physicsSystem.update(entities, FIXED_DT);
        collisionSystem.update(entities, tilemap);
        entitySystem.update(entities, FIXED_DT);
        eventBus.dispatch();            // Flush all pending events
        accumulator -= FIXED_DT;
    }

    // 5. Render — interpolate between 2 physics frames
    float alpha = accumulator / FIXED_DT;   // 0.0 → 1.0
    renderSystem.render(entities, camera, hud, alpha);

    // 6. FPS cap (optional, to prevent CPU running at 100%)
    SDL_Delay(1);  // Yield CPU, avoid busy-wait
}
```

---

## 4.2 Detailed Logic for Each Step in the Loop

### STEP 1 — Input Poll (once per real frame)

```
inputSystem.pollEvents():
  SDL_PumpEvents()
  SDL_PollEvent(&event):
    SDL_QUIT          → game.running = false
    SDL_KEYDOWN/UP    → update keystates[scancode]
    SDL_CONTROLLERBUTTONDOWN/UP → update controller states

inputSystem.buildInputState(out InputState):
  InputState {
    moveLeft       : bool  ← LEFT or A pressed
    moveRight      : bool  ← RIGHT or D pressed
    jump           : bool  ← SPACE or W or UP pressed
    jumpJustPressed: bool  ← true only on first frame of press
    run            : bool  ← SHIFT or Z held
    shoot          : bool  ← SHIFT or Z just pressed (FireMario)
    crouch         : bool  ← DOWN or S pressed
    pause          : bool  ← ESC just pressed
  }
```

### STEP 2 — Physics Update (fixed 60Hz)

```
physicsSystem.update(entities, FIXED_DT):
  for each entity in entities:
    if entity has no PhysicsComponent → skip

    // 2a. Apply gravity
    if not entity.grounded:
        entity.velocity.y += GRAVITY * entity.gravity_scale * FIXED_DT

    // 2b. Cap fall speed
    entity.velocity.y = min(entity.velocity.y, MAX_FALL_SPEED)

    // 2c. Apply horizontal friction when grounded and not moving
    if entity.grounded and entity is not being moved:
        entity.velocity.x *= (1.0f - FRICTION * FIXED_DT)
        if abs(entity.velocity.x) < 0.01f:
            entity.velocity.x = 0

    // 2d. Update position (simple Euler integration)
    entity.position += entity.velocity * FIXED_DT

    // 2e. Reset grounded flag (CollisionSystem will set it again)
    entity.grounded = false
```

### STEP 3 — Collision Detection & Resolution

```
collisionSystem.update(entities, tilemap):

  // 3a. Mario vs Tilemap (highest priority)
  for each solid tile near Mario (using SpatialHash to avoid checking all):
    if AABB_overlap(mario.hitbox, tile.rect):
      MTV = computeMTV(mario.hitbox, tile.rect)
      apply(MTV) to mario.position
      if MTV.y < 0:  mario.velocity.y = 0; mario.grounded = true
      if MTV.y > 0:  mario.velocity.y = 0  (head hit → stop)
      if MTV.x != 0: mario.velocity.x = 0  (wall hit → stop)

      // Check tile type
      if tile.type == QUESTION and MTV.y > 0:
          eventBus.publish(BLOCK_HIT, tile.position, tile.itemType)
          tile.type = EMPTY_BLOCK

      if tile.type == BRICK and MTV.y > 0 and mario.isSuperOrAbove:
          eventBus.publish(BLOCK_BROKEN, tile.position)
          tile.active = false
          spawnBrickParticles(tile.position)

  // 3b. Enemy vs Tilemap
  for each enemy:
    resolve enemy vs solid tiles → enemy.grounded, reverse on wall contact

  // 3c. Mario vs Enemy
  for each enemy:
    if not AABB_overlap(mario.hitbox, enemy.hitbox): continue
    side = detectSide(mario, enemy)

    if side == TOP and mario.velocity.y > 0:
        // Mario stomps enemy from above
        eventBus.publish(ENEMY_STOMPED, enemy.id, enemy.position, enemy.scoreValue)
        enemy.onStomped()
        mario.velocity.y = STOMP_BOUNCE  (-4.0f, slight bounce after stomp)

    elif not mario.isInvincible and not mario.invulTimer > 0:
        // Enemy touches Mario's body
        eventBus.publish(MARIO_GOT_HIT, mario.position)
        mario.takeDamage()

  // 3d. Fireball vs Enemy
  for each fireball in fireballs:
    for each enemy:
      if AABB_overlap:
        eventBus.publish(FIREBALL_HIT_ENEMY, enemy.id)
        enemy.onFireball()
        fireball.active = false

  // 3e. Fireball vs Tile
  for each fireball:
    if hits solid tile on left/right: fireball.active = false
    if hits solid tile on top (floor): fireball bounces (velocity.y *= -1, bounceCount++)
    if fireball.bounceCount >= 3:      fireball.active = false

  // 3f. Mario vs Item
  for each item:
    if AABB_overlap(mario.hitbox, item.hitbox):
      eventBus.publish(ITEM_COLLECTED, item.type)
      item.onPickup(mario)
      item.active = false

  // 3g. Shell vs Enemy
  for each sliding shell:
    for each enemy:
      if AABB_overlap:
        enemy.onShell()
        comboCount++
        eventBus.publish(ENEMY_KILLED, combo_score)

  // 3h. Mario vs Coin
  for each coin:
    if AABB_overlap:
      eventBus.publish(MARIO_COIN_COLLECTED, position)
      coin.active = false
```

### STEP 4 — Entity Update (AI, FSM, Animation, Timers)

```
entitySystem.update(entities, FIXED_DT):

  // 4a. Mario update
  mario.handleMovement(inputState, FIXED_DT):
    if moveLeft:  velocity.x -= acceleration * FIXED_DT; facingRight = false
    if moveRight: velocity.x += acceleration * FIXED_DT; facingRight = true
    velocity.x = clamp(velocity.x, -maxSpeed, maxSpeed)
    if not moveLeft and not moveRight:
      velocity.x = approach(velocity.x, 0, deceleration * FIXED_DT)

  mario.handleJump(inputState, FIXED_DT):
    // Coyote time timer
    if mario.grounded: coyoteTimer = COYOTE_TIME
    else: coyoteTimer -= FIXED_DT

    // Jump buffer timer
    if inputState.jumpJustPressed: jumpBufferTimer = JUMP_BUFFER_TIME
    else: jumpBufferTimer -= FIXED_DT

    // Execute jump
    canJump  = (mario.grounded or coyoteTimer > 0)
    wantsJump = (jumpBufferTimer > 0)
    if canJump and wantsJump:
      mario.velocity.y = JUMP_INITIAL_VELOCITY
      mario.grounded = false
      coyoteTimer = 0; jumpBufferTimer = 0
      jumpHoldTimer = JUMP_HOLD_MAX_TIME
      eventBus.publish(MARIO_JUMPED)

    // Variable jump — reduce gravity while holding button
    if inputState.jump and jumpHoldTimer > 0 and mario.velocity.y < 0:
      mario.gravity_scale = JUMP_HOLD_GRAVITY_SCALE  // 0.5
      jumpHoldTimer -= FIXED_DT
    else:
      mario.gravity_scale = 1.0
      jumpHoldTimer = 0

  // 4b. Enemy AI update
  for each enemy:
    enemy.onUpdateAI(FIXED_DT)  // Virtual — each enemy type handles its own logic

  // 4c. MarioFSM — handle invulnerability timer
  if mario.invulTimer > 0:
    mario.invulTimer -= FIXED_DT
    mario.visible = (frame % 4 < 2)  // Flash sprite when invulnerable
  else:
    mario.visible = true

  // 4d. StarPower timer
  if mario.fsm.state == INVINCIBLE:
    mario.starTimer -= FIXED_DT
    if mario.starTimer <= 0:
      mario.fsm.transition(previousState)
      audioManager.resumePreviousBGM()

  // 4e. Animation update
  for each entity with AnimationComponent:
    anim.timer -= FIXED_DT
    if anim.timer <= 0:
      anim.currentFrame = (anim.currentFrame + 1) % anim.frameCount
      anim.timer = anim.frameTime
      if anim.currentFrame == 0 and not anim.loop:
        anim.finished = true

  // 4f. Camera update
  camera.target = mario.position
  camera.x = clamp(mario.x - SCREEN_W/2, 0, map.pixelWidth - SCREEN_W)
  camera.x = max(camera.x, camera.x_min)  // Never scroll backward

  // 4g. Countdown timer
  gameTimer -= FIXED_DT
  if gameTimer <= 100 and not timerWarningPlayed:
    audioManager.playHurryBGM()
    timerWarningPlayed = true
  if gameTimer <= 0:
    eventBus.publish(MARIO_DIED)

  // 4h. Remove inactive entities
  entities.erase_if(not entity.active)

  // 4i. Particle update
  particleSystem.update(FIXED_DT)
```

### STEP 5 — Event Dispatch

```
eventBus.dispatch():
  for each event in pendingQueue:
    for each listener subscribed to event.type:
      listener.onEvent(event)

  Event processing order (to prevent circular dependencies):
     1. MARIO_DIED        → LivesSystem (lives--)
     2. ENEMY_KILLED      → ScoreSystem (score++)
     3. SCORE_ADDED       → HUD (display update) + ParticleSystem
     4. ITEM_COLLECTED    → MarioFSM (state change)
     5. BLOCK_HIT         → EntityFactory (spawn item/coin)
     6. BLOCK_BROKEN      → ParticleSystem (brick shards)
     7. LEVEL_COMPLETE    → SceneManager (transition)
     8. GAME_OVER         → SceneManager (game over screen)
     9. TIMER_WARNING     → AudioManager (hurry BGM)
    10. COIN_COUNT_CHANGED → check 100 coins → +1 life

  pendingQueue.clear()
```

### STEP 6 — Render (Alpha Interpolation)

```
renderSystem.render(entities, camera, hud, alpha):

  // 6a. Clear screen
  SDL_SetRenderDrawColor(renderer, bg_color)
  SDL_RenderClear(renderer)

  // 6b. Compute interpolated position (smooth rendering between 2 physics steps)
  for each entity:
    renderPos = entity.prevPosition * (1 - alpha) + entity.position * alpha

  // 6c. Frustum culling — only draw entities inside camera view
  cameraRect = {camera.x, camera.y, SCREEN_W, SCREEN_H}
  visibleEntities = entities.filter(AABB_overlap(entity.hitbox + renderPos, cameraRect))

  // 6d. Sort by layer (draw from lowest layer to highest)
  sort visibleEntities by entity.layer ascending

  // 6e. Draw tilemap background (before entities)
  tilemap.render(renderer, camera)

  // 6f. Draw entities
  for each visibleEntity (sorted):
    srcRect = anim.getCurrentFrame()
    dstRect = {renderPos.x - camera.x, renderPos.y - camera.y, w, h}
    SDL_RenderCopyEx(renderer, texture, srcRect, dstRect, 0, NULL, flip)

  // 6g. Draw particles (always above entities)
  particleSystem.render(renderer, camera)

  // 6h. Draw HUD (unaffected by camera, always top layer)
  hud.render(renderer, score, coins, world, lives, timer)

  // 6i. Debug overlay (if F1 enabled)
  if debugMode:
    for each entity: drawRect(entity.hitbox, GREEN)
    drawText("FPS: " + fps, RED)
    drawText("Entities: " + entityCount)

  // 6j. Present
  SDL_RenderPresent(renderer)
```

---

# PART V — COMPLETE ENTITY LIST

---

## 5.1 Player Entities

| Entity | File | Description |
|---|---|---|
| `Mario` | `mario/Mario.h` | Main character, controlled by keyboard |

## 5.2 Enemy Entities — Complete List

| Entity | File | Environment | Stompable | Fireball | Points |
|---|---|---|---|---|---|
| `Goomba` | `enemies/Goomba.h` | Overworld, Castle | ✅ → flattens | ✅ | 100 |
| `KoopaTroopa (Green)` | `enemies/KoopaTroopa.h` | Overworld | ✅ → Shell | ✅ | 100 |
| `KoopaTroopa (Red)` | `enemies/KoopaTroopa.h` | Overworld | ✅ → Shell | ✅ | 100 |
| `KoopaShell` | `enemies/KoopaShell.h` | Overworld | ✅ (kick) | ✅ | 400+ |
| `PiranhaPlant` | `enemies/PiranhaPlant.h` | Pipes | ❌ | ✅ | 200 |
| `HammerBro` | `enemies/HammerBro.h` | Overworld, Castle | ✅* | ✅ | 1000 |
| `Lakitu` | `enemies/Lakitu.h` | Sky | ✅ | ✅ | 800 |
| `Spiny` | `enemies/Spiny.h` | Overworld | ❌ (spiky) | ✅ | 200 |
| `BulletBill` | `enemies/BulletBill.h` | Overworld | ✅ | ✅ | 200 |
| `BulletBillLauncher` | `enemies/BulletBillLauncher.h` | Wall | ❌ | ❌ | 0 |
| `CheepCheep (Straight)` | `enemies/CheepCheep.h` | Water | ✅ | ✅ | 200 |
| `CheepCheep (Leaping)` | `enemies/CheepCheep.h` | Air (jumps up) | ✅ | ✅ | 200 |
| `Blooper` | `enemies/Blooper.h` | Water | ✅ | ✅ | 200 |
| `FireBar` | `enemies/FireBar.h` | Castle | ❌ (static) | ❌ | 0 |
| `FakeBowser` | `enemies/FakeBowser.h` | Castle (1-4 → 7-4) | ❌ | ✅ (1 hit) | 5000 |
| `Bowser (Real)` | `enemies/Bowser.h` | Castle (8-4) | ❌ | ✅ (5 hits) | — |

> *HammerBro can be stomped but is difficult because it jumps constantly

## 5.3 Item Entities

| Entity | File | Spawns From | Effect |
|---|---|---|---|
| `Mushroom` | `items/Mushroom.h` | Question Block (Small Mario) | Small → Super |
| `FireFlower` | `items/FireFlower.h` | Question Block (Super/Fire Mario) | Super → Fire |
| `StarPower` | `items/StarPower.h` | Special Question Block | Invincible 10s |
| `OneUpMushroom` | `items/OneUpMushroom.h` | Special hidden block | +1 Life |
| `Coin` | (inline in TileMap) | Question Block / Air / Brick | +200 pts, +1 coin |

## 5.4 Tile Entities / Block Types

| Tile | TileType Enum | Behavior |
|---|---|---|
| Solid Ground | `SOLID` | Blocks all directions |
| Sky / Air | `AIR` | No collision |
| Brick | `BRICK` | Breaks if Super+ hits from below |
| Question Block | `QUESTION` | Releases item/coin when hit from below, becomes EMPTY |
| Empty Block | `EMPTY_BLOCK` | Solid but releases nothing |
| Pipe Top | `PIPE_TOP` | Solid + Mario can enter if exit exists |
| Pipe Body | `PIPE_BODY` | Solid |
| Cloud | `CLOUD` | One-way platform (blocks only from above) |
| Water Surface | `WATER_TOP` | One-way, activates underwater physics |
| Water | `WATER` | Reduced gravity, allows swimming |
| Lava | `LAVA` | Contact → instant death |
| Death Zone | `DEATH_ZONE` | Fall in → death |
| Flag Pole | `FLAG_POLE` | Level end |
| Castle | `CASTLE` | Destination after flagpole |
| Axe | `AXE` | Triggers bridge collapse (Castle level) |
| Bridge | `BRIDGE` | Collapses when Axe is triggered |
| Hidden Block | `HIDDEN_BLOCK` | Becomes SOLID when Mario jumps into it |
| Note Block | `NOTE_BLOCK` | Launches Mario upward when jumped on |
| Coin Block | `COIN_BLOCK` | Holds many coins, releases one per hit |

## 5.5 Projectile Entities

| Entity | File | Fired By | Physics | Damages |
|---|---|---|---|---|
| `Fireball` | `projectiles/Fireball.h` | Fire Mario | Bounces on ground | Enemies |
| `Hammer` | `projectiles/Hammer.h` | Hammer Bro | Arc (gravity) | Mario |
| `BowserFire` | `projectiles/BowserFire.h` | Bowser | Straight horizontal | Mario |
| `Spiny` (after thrown) | `enemies/Spiny.h` | Lakitu | Falls → walks | Mario |

## 5.6 Visual-Only Entities (Particles)

| Particle Type | Triggered When |
|---|---|
| `SCORE_POPUP` | Stomp enemy, collect coin → score number floats up |
| `BRICK_SHARD` | Brick breaks → shards fly in 4 directions |
| `COIN_SPARKLE` | Collect coin → sparkle effect |
| `DUST_CLOUD` | Mario skids, lands after jump |
| `STAR_SPARKLE` | Mario is Invincible |
| `DEATH_STAR` | Mario dies → small stars orbit around |
| `EXPLOSION` | Bowser is destroyed |
| `SMOKE` | FireBar, Bowser Fire |

---

# PART VI — DETAILED WORLD STRUCTURE (8 WORLDS)

---

## 6.0 General Conventions

```
Each World has 4 Levels:
  X-1 : Overworld (plains, teaches basic mechanics)
  X-2 : Underground OR Underwater (varies by World)
  X-3 : Sky/Bridge level (special platforms)
  X-4 : Castle (Boss Bowser/FakeBowser)

Symbol conventions:
  [?] = Question Block
  [B] = Brick Block
  [P] = Pipe (height varies)
  [C] = Coin
  [E] = Enemy
  [*] = Special point (trap, secret)
```

---

## 6.1 WORLD 1 — Overworld / Tutorial World

```
Theme     : Overworld (green, sunny)
BGM       : overworld_theme
Difficulty: ⭐ Easiest — teaches basics

─── World 1-1 ──────────────────────────────────
Layout overview:
  Start     : 1 Goomba nearby → "jump" tutorial
  Section 1 : Plains, first pipe, [?] block with Mushroom
  Section 2 : Ascending stairs, many Goombas
  Section 3 : Tall pipes, Piranha Plants
  Warp Zone : 4th pipe → Warp to World 2/3/4
  End       : Standard flagpole

Item placement:
  [?] Block #1: Mushroom (first Super Mario power-up)
  [?] Block #2-5: Coins
  [?] Block #6: Star (secret inside Brick cluster)
  Hidden Block: 1-Up Mushroom (at iconic hidden location)

Enemy spawns:
  Goomba × 8 (solo and groups of 2-3)
  Koopa Troopa × 0 (not introduced yet)
  Piranha Plant × 2 (inside pipes)

Design Goal: Teaches the player about:
  - Question Blocks
  - Jumping over Goombas
  - Mushroom = grow bigger
  - Pipes (and Piranha Plant warning)

─── World 1-2 ──────────────────────────────────
Theme     : Underground (purple/dark blue background)
BGM       : underground_theme
Features  : Underground cave, many Brick blocks, low ceiling

Layout:
  Section 1 : Narrow corridor, many Goombas
  Section 2 : Area with elevated [?] blocks
  Warp Zone : End of level → Warp to World 6/7/8 (most famous warp)
  End       : Escalator stairs up, exit to overworld

New mechanics:
  - Breaking Bricks (requires Super Mario)
  - Low ceiling → Mario cannot reach full jump height

─── World 1-3 ──────────────────────────────────
Theme     : Sky/Cloud (sky blue background)
BGM       : overworld_theme
Features  : Floating cloud platforms, one-way platforms, wooden bridge

Layout:
  - All platforms are floating clouds (white)
  - Koopa Troopa introduced for the first time
  - Moving platforms (side to side)
  - Requires precise jump timing

New enemies: Koopa Troopa (green) × 4

─── World 1-4 ──────────────────────────────────
Theme     : Castle (dark brick, fire)
BGM       : castle_theme
Features  : Simple maze, fire hazards, Boss

Layout:
  Section 1 : Straight corridor with FireBars
  Section 2 : Platforms over lava
  Boss Arena: FakeBowser (actually a Goomba)
              To clear: Touch Axe → Bridge collapses → FakeBowser falls in lava
  Reward    : Toad NPC: "Thank you Mario! But our princess is in another castle!"

Boss: FakeBowser (actually a Goomba wearing Bowser's sprite)
  → 1 Fireball = Dies instantly (reveals true form)
  → Axe = Bridge collapses
```

---

## 6.2 WORLD 2 — Desert / Night World

```
Theme     : Dark night / Desert (black background, cacti)
BGM       : overworld_theme (night version, different palette)
Difficulty: ⭐⭐ Easy-Medium

─── World 2-1 ──────────────────────────────────
Features  : Dark night, Red Koopa introduced for the first time
New enemies: Koopa Troopa (Red) — does not fall off edges
             BulletBill — flies horizontally from Launcher
             CheepCheep (Leaping) — fish that jumps up from water

New mechanics:
  - BulletBill: No warning, appears from the right side of screen
  - Red Koopa reverses before cliff edges (more dangerous)

─── World 2-2 ──────────────────────────────────
Theme     : Underwater
BGM       : underwater_theme
Features  : Underwater physics — reduced gravity, swim up/down

Underwater physics:
  gravity_scale = 0.3 (slow fall)
  jump → swim (multiple times, no ground needed)
  max_fall_speed = 1.5 (slow fall)

New enemies: Blooper (squid that chases Mario)
             CheepCheep (Straight) — swims horizontally

Design:
  - No flagpole → reach PIPE at end of level
  - Must constantly dodge Bloopers
  - Narrow coral reef passages

─── World 2-3 ──────────────────────────────────
Theme     : Bridge / Treetop (elevated bridge)
BGM       : overworld_theme
Features  : Narrow platforms, many gaps, Hammer Bro

New enemies: Hammer Bro — first appearance, very dangerous
Design    : Hard to dodge hammers, need to run fast

─── World 2-4 ──────────────────────────────────
Boss: FakeBowser (actually a Koopa Troopa)
      → 5 Fireballs or 1 Axe
```

---

## 6.3 WORLD 3 — Ocean / Island World

```
Theme     : Ocean island (ocean blue background)
BGM       : overworld_theme
Difficulty: ⭐⭐ Medium

─── World 3-1 ──────────────────────────────────
Features  : Islands floating over sea, many Koopas, moving platforms
Enemies   : Koopa (Red and Green), more than W1-3

Moving Platform mechanics:
  - Platform moves along X axis (back and forth)
  - Platform moves along Y axis (up and down)
  - Standing on moving platform → Mario inherits platform's velocity

─── World 3-2 ──────────────────────────────────
Theme     : Underwater
Similar to 2-2 but harder: more Bloopers, narrower corridors

─── World 3-3 ──────────────────────────────────
Theme     : Sky + Bridge
Features  : Combination of clouds and wooden bridge
Enemies   : More Hammer Bros

─── World 3-4 ──────────────────────────────────
Boss: FakeBowser (Buzzy Beetle or Koopa)
```

---

## 6.4 WORLD 4 — Giant World

```
Theme     : Plains but EVERYTHING IS DOUBLE-SIZED
BGM       : overworld_theme
Difficulty: ⭐⭐⭐ Medium-Hard
Features  : Tile size doubled → Mario feels smaller

Technical Note:
  Tile size = 32×32 px (instead of 16×16)
  Enemies also scaled up accordingly
  Mario hitbox unchanged → harder to dodge

─── World 4-1 ──────────────────────────────────
  Giant Goomba × 4 (double size, requires better jump skills)
  Giant Pipe → Giant Piranha Plant (larger hitbox)

─── World 4-2 ──────────────────────────────────
  Giant Underground
  Low ceiling + Giant Brick → Breaking Bricks clears the path

─── World 4-3 ──────────────────────────────────
  Giant Sky — Large cloud platforms, longer jump distances

─── World 4-4 ──────────────────────────────────
Boss: Double-sized FakeBowser (actually a Giant Goomba)
```

---

## 6.5 WORLD 5 — Sky World

```
Theme     : Night + Sky
BGM       : overworld_theme (night version)
Difficulty: ⭐⭐⭐ Hard

─── World 5-1 ──────────────────────────────────
  Entirely on clouds, complex one-way platforms
  Lakitu introduced — throws Spinies from above
  Skill needed: Handle Lakitu while dodging narrow platforms

─── World 5-2 ──────────────────────────────────
  Underground with many Hammer Bros
  Narrow corridors, hard to dodge hammers

─── World 5-3 ──────────────────────────────────
  Nighttime bridge level
  CheepCheep leaping from below + HammerBro on bridge

─── World 5-4 ──────────────────────────────────
  Complex castle — many identical-looking corridors (fake maze)
  Must remember the correct path
Boss: FakeBowser
```

---

## 6.6 WORLD 6 — Ice / Mountain World

```
Theme     : Ice and snow, mountains
BGM       : overworld_theme (white/ice blue version)
Difficulty: ⭐⭐⭐⭐ Hard

─── World 6-1 ──────────────────────────────────
  Slippery ground (Ice physics):
    friction = very low → Mario slides a long distance
    Momentum is harder to control
  Enemy: Buzzy Beetle (immune to Fireballs, only stompable or via Shell)

Ice Physics:
  ice_friction = 0.02 (vs 0.5 on normal ground)
  → Player must anticipate stopping well in advance

─── World 6-2 ──────────────────────────────────
  Nighttime underwater — dark and hard to see
  More Bloopers, moving faster

─── World 6-3 ──────────────────────────────────
  Sky level — narrow ice platforms
  Ice physics + small platforms = extremely difficult

─── World 6-4 ──────────────────────────────────
  Castle — more fire hazards, double FireBar (2 bars rotating opposite directions)
Boss: FakeBowser
```

---

## 6.7 WORLD 7 — Pipe World

```
Theme     : Pipe forest
BGM       : overworld_theme
Difficulty: ⭐⭐⭐⭐ Very Hard

─── World 7-1 ──────────────────────────────────
  Pipes EVERYWHERE — pipe tops are the main platforms
  Piranha Plants in most pipes
  Precise jump distances between pipe tops required

─── World 7-2 ──────────────────────────────────
  Final underground level — combines all enemy types
  Hammer Bro + Lakitu + Spiny all at once

─── World 7-3 ──────────────────────────────────
  Sky level — cloud platforms + aerial pipes
  Elevated Piranha Plants are more dangerous

─── World 7-4 ──────────────────────────────────
  MAZE Castle — most complex of all 7 castles
  Many wrong paths loop back to start
  Must memorize the correct sequence:
    Go straight → turn left → go straight → climb stairs
Boss: FakeBowser (last one — actually a Hammer Bro)
```

---

## 6.8 WORLD 8 — FINAL WORLD (Dark Castle)

```
Theme     : Dark castle, red sky
BGM       : castle_theme (ALL 4 levels are castle-themed)
Difficulty: ⭐⭐⭐⭐⭐ Hardest

─── World 8-1 ──────────────────────────────────
  Plains BUT every enemy type appears
  No checkpoints
  Tank + Bowser Ship (visual platforms)
  Many Hammer Bros, continuous BulletBills

─── World 8-2 ──────────────────────────────────
  Final Underwater — fast Bloopers, dense CheepCheeps

─── World 8-3 ──────────────────────────────────
  Final Bridge — double FireBar + HammerBro + no cover
  Extremely hard to dodge

─── World 8-4 ──────────────────────────────────
  TRUE MAZE Castle
  Multiple loops — wrong path → sent back to section start
  Correct sequence must be memorized precisely

  Before Boss:
    Maximum FireBars
    Lava on all sides
    Hammer Bros guarding

  BOSS: THE REAL BOWSER
    HP = 5 (Fireball only)
    Shoots fire every 2s
    Throws Hammers × 3 every 5s
    Moves faster at lower HP
    Cannot be jumped over (too tall)

    How to win:
    Option A: 5 Fireballs (FireMario) → Bowser dies
              → Reveals true form: REAL BOWSER (not a disguise)
              → Screen: "Congratulations!"
    Option B: Touch Axe → Bridge collapses → Bowser falls in lava
              → Game clear

  ENDING:
    Mario rescues Princess Peach
    "Thank you Mario! Your quest is over."
    Credits roll
    → Returns to World 1-1 (harder difficulty — A-Button mode)
```

---

## 6.9 Summary of All 8 Worlds

| World | Theme | W-2 Type | Difficulty | Boss | Fake/Real |
|---|---|---|---|---|---|
| 1 | Overworld | Underground | ⭐ | Goomba | Fake |
| 2 | Night/Desert | **Underwater** | ⭐⭐ | Koopa | Fake |
| 3 | Ocean/Island | **Underwater** | ⭐⭐ | Beetle | Fake |
| 4 | **Giant** | Underground | ⭐⭐⭐ | Giant Goomba | Fake |
| 5 | Night Sky | Underground | ⭐⭐⭐ | Koopa | Fake |
| 6 | **Ice/Mountain** | **Underwater** | ⭐⭐⭐⭐ | Koopa | Fake |
| 7 | **Pipe Forest** | Underground | ⭐⭐⭐⭐ | HammerBro | Fake |
| 8 | **Dark Final** | **Underwater** | ⭐⭐⭐⭐⭐ | **BOWSER** | **REAL** |

---

# PART VII — RISK MANAGEMENT & EXCEPTION HANDLING

---

## 7.1 Complete Risk Register

| ID | Risk | Category | Probability | Severity | Mitigation |
|---|---|---|---|---|---|
| R01 | Memory leak from Entity not deleted | Memory | 🔴 High | 🔴 High | `unique_ptr`, deferred deletion |
| R02 | Dangling pointer after entity death | Memory | 🔴 High | 🔴 High | `active` flag + remove at end of frame |
| R03 | Physics incorrect during sudden FPS drop | Physics | 🟡 Medium | 🔴 High | Cap `frameTime` to max 250ms |
| R04 | Mario tunnels through thin tile at high speed | Physics | 🟡 Medium | 🔴 High | Swept AABB or limit max velocity |
| R05 | Asset file missing on load | IO | 🟡 Medium | 🟡 Medium | try-catch + fallback placeholder |
| R06 | JSON parse error (corrupt map file) | IO | 🟡 Medium | 🟡 Medium | Validate schema + fallback level |
| R07 | SDL2 fails to initialize (missing driver) | Platform | 🟢 Low | 🔴 High | Check SDL_Init return code, exit gracefully |
| R08 | Audio device unavailable | Audio | 🟡 Medium | 🟢 Low | Continue running without audio |
| R09 | Integer overflow in score | Logic | 🟢 Low | 🟢 Low | `uint32_t`, cap at 9,999,999 |
| R10 | Camera scrolls backward (bug) | Logic | 🟡 Medium | 🟡 Medium | `camera.x = max(camera.x, camera.x_min)` |
| R11 | Entity spawns outside map boundary | Logic | 🟡 Medium | 🟡 Medium | Validate spawn points from JSON |
| R12 | Object Pool overflow (too many fireballs) | Memory | 🟢 Low | 🟡 Medium | `if pool.full(): return` (don't spawn more) |
| R13 | Save file corrupted | IO | 🟢 Low | 🟡 Medium | Validate save file, reset if invalid |
| R14 | Race condition if threading added later | Thread | 🟢 Low | 🔴 High | Document clearly as single-thread, use mutex when threading added |
| R15 | Piranha Plant exits pipe when it should not | Logic | 🟡 Medium | 🟡 Medium | Check mario_near_pipe every frame |

---

## 7.2 Exception Handling Mechanisms

```cpp
// Standard error handling patterns

// 7.2.1 — Asset loading with fallback
SDL_Texture* AssetManager::loadTexture(const string& path) {
    SDL_Texture* texture = IMG_LoadTexture(renderer, path.c_str());
    if (!texture) {
        Logger::error("Cannot load: " + path + " | " + IMG_GetError());
        return placeholderTexture;  // Return placeholder instead of crashing
    }
    return texture;
}

// 7.2.2 — JSON loading with validation
bool LevelLoader::loadLevel(const string& path, TileMap& out) {
    ifstream file(path);
    if (!file.is_open()) {
        Logger::error("Cannot open map file: " + path);
        return loadDefaultLevel(out);  // Fallback
    }
    try {
        json data = json::parse(file);
        // Validate required fields
        if (!data.contains("tiles") || !data.contains("spawn")) {
            throw std::runtime_error("Invalid map format");
        }
        // Parse...
    } catch (const exception& e) {
        Logger::error("Map parse error: " + string(e.what()));
        return loadDefaultLevel(out);
    }
    return true;
}

// 7.2.3 — Object Pool overflow handling
Fireball* FireballPool::allocate() {
    for (auto& obj : pool) {
        if (!obj.active) {
            obj.reset();
            return &obj;
        }
    }
    Logger::warn("FireballPool overflow — pool size: " + to_string(POOL_SIZE));
    return nullptr;  // Do not spawn more, do not crash
}

// Caller must check for null:
if (Fireball* fb = pool.allocate()) {
    fb->init(position, direction);
}

// 7.2.4 — Velocity cap (prevent wall tunneling)
void PhysicsSystem::update(Entity& e, float dt) {
    // Cap velocity before applying to prevent tunnel-through
    e.velocity.x = clamp(e.velocity.x, -MAX_VELOCITY_X, MAX_VELOCITY_X);
    e.velocity.y = clamp(e.velocity.y, -MAX_VELOCITY_Y, MAX_VELOCITY_Y);
    e.position += e.velocity * dt;
}
```

---

## 7.3 Logging Strategy

```
Log Levels:
  DEBUG : All physics events (enabled in dev builds only)
  INFO  : Level load, state change, score milestone
  WARN  : Pool overflow, asset fallback, slow frame
  ERROR : File not found, SDL error, invalid JSON

Log Format:
  [2026-05-30 12:30:45.123] [INFO]  PlayState: Loaded world_1_1.json (0.023s)
  [2026-05-30 12:30:46.456] [WARN]  FireballPool: overflow (pool=2)
  [2026-05-30 12:30:47.789] [ERROR] AssetManager: Cannot load 'sprites/mario.png'

File Output:
  logs/game.log (append mode, rotate when > 10MB)
```

---

# PART VIII — TESTING STRATEGY & EDGE CASES

---

## 8.1 Detailed Unit Test Files

### test_physics.cpp

```
TC_PHY_001: Gravity applied correctly — velocity.y increases by GRAVITY each frame
TC_PHY_002: Terminal velocity — velocity.y does not exceed MAX_FALL_SPEED
TC_PHY_003: Grounded = true after collision resolves with tile below
TC_PHY_004: Velocity.y = 0 when grounded (gravity does not accumulate)
TC_PHY_005: Horizontal friction — velocity.x decreases when key released, reaches 0
TC_PHY_006: Fixed timestep — physics output consistent regardless of frameTime variation
TC_PHY_007: Jump initial velocity applied correctly when jumpJustPressed
TC_PHY_008: Variable jump — gravity_scale = 0.5 when holding jump, = 1.0 when released
TC_PHY_009: Velocity capped when exceeding MAX_VELOCITY_X
TC_PHY_010: Ice friction — velocity.x decreases more slowly on ice tile
```

### test_collision.cpp

```
TC_COL_001: AABB overlap — 2 overlapping rects return true
TC_COL_002: AABB no-overlap — 2 non-overlapping rects return false
TC_COL_003: MTV — collision from above (bottom hit) → MTV.y < 0, entity pushed up
TC_COL_004: MTV — collision from below (top hit) → MTV.y > 0, entity pushed down
TC_COL_005: MTV — collision from left → MTV.x > 0, entity pushed right
TC_COL_006: MTV — collision from right → MTV.x < 0, entity pushed left
TC_COL_007: Corner case — smallest overlap axis determines resolution axis
TC_COL_008: One-way platform — entity passes through from below
TC_COL_009: One-way platform — entity stops when coming from above
TC_COL_010: Multiple tile collision — entity between 2 tiles resolved correctly
TC_COL_011: Corner correction — Mario overlaps 2px at tile corner → pushed sideways
TC_COL_012: Stomp detection — Mario velocity.y > 0 and hits TOP of enemy → stomp
TC_COL_013: Side hit — Mario velocity.y = 0 and hits SIDE of enemy → take damage
TC_COL_014: Fireball vs Tile — hit bottom → bounce, hit side → destroy
TC_COL_015: Fireball vs Enemy → enemy hit, fireball destroyed
```

### test_mario_fsm.cpp

```
TC_FSM_001: Small → Super (collected Mushroom)
TC_FSM_002: Super → Fire (collected FireFlower)
TC_FSM_003: Fire → Super (hit once)
TC_FSM_004: Super → Small (hit once)
TC_FSM_005: Small + hit → DYING state
TC_FSM_006: Small + Star → INVINCIBLE (keeps SMALL size)
TC_FSM_007: Super + Star → INVINCIBLE (keeps SUPER size)
TC_FSM_008: INVINCIBLE → 10s elapsed → returns to previous state
TC_FSM_009: INVINCIBLE + hit → does not lose state, does not flash
TC_FSM_010: After losing state (Super→Small), invulnerable 2s (cannot be hit consecutively)
TC_FSM_011: Mushroom when already Fire → no effect (already at max state)
TC_FSM_012: FireFlower when Small → Mushroom spawned instead (per NES logic)
```

### test_animation.cpp

```
TC_ANI_001: Frame advances correctly after frameTime expires
TC_ANI_002: Loop animation — returns to frame 0 after last frame
TC_ANI_003: Non-loop animation — stops at last frame, sets finished=true
TC_ANI_004: Animation switch — frame resets to 0 when switching to new animation
TC_ANI_005: Horizontal flip — facingRight=false → SDL_FLIP_HORIZONTAL
TC_ANI_006: Animation frame count matches sprite sheet definition
```

### test_tilemap_loader.cpp

```
TC_MAP_001: Load valid JSON → TileMap populated correctly
TC_MAP_002: File does not exist → fallback level, no crash
TC_MAP_003: JSON missing "tiles" key → exception handled, fallback
TC_MAP_004: getTileAt(x, y) within bounds → returns correct tile
TC_MAP_005: getTileAt(-1, 0) out of bounds → returns AIR, no crash
TC_MAP_006: getTileAt(x > width) out of bounds → returns AIR, no crash
TC_MAP_007: Entity spawn list from JSON → factory creates correct entity type
TC_MAP_008: Question Block with item → hasItem=true, itemType correct
TC_MAP_009: Pipe exit mapping → entry pipe correctly linked to exit pipe
TC_MAP_010: World boundary → death zone below map
```

### test_object_pool.cpp

```
TC_POL_001: Allocate from pool → returns non-null, object.active=true
TC_POL_002: Release → object.active=false, slot can be reused
TC_POL_003: Allocate → Release → Allocate again → same slot reused
TC_POL_004: Pool full (all slots active) → allocate returns nullptr
TC_POL_005: Pool size matches POOL_SIZE template param
TC_POL_006: Reset after allocate → object in clean state
```

### test_event_bus.cpp

```
TC_EVT_001: Subscribe → publish → listener is called
TC_EVT_002: Multiple listeners on same event → all are called
TC_EVT_003: Unsubscribe → listener NOT called after unsubscribing
TC_EVT_004: Publish with no listeners → no crash
TC_EVT_005: Event queue flush → pendingQueue empty after dispatch
TC_EVT_006: Event data payload is correct type when received
```

### test_enemy_ai.cpp

```
TC_AI_001: Goomba walks left → hits wall → reverses to right
TC_AI_002: Goomba (Green) reaches cliff edge → continues walking (falls off)
TC_AI_003: Koopa (Red) reaches cliff edge → reverses direction (does NOT fall)
TC_AI_004: Koopa stomped → ShellState = IN_SHELL, stops moving
TC_AI_005: Shell IN_SHELL → kicked → ShellState = SLIDING, velocity.x = SHELL_SPEED
TC_AI_006: Shell SLIDING → hits wall → reverses direction
TC_AI_007: Shell SLIDING → 5s with no contact → Koopa REVIVES
TC_AI_008: PiranhaPlant → Mario farther than 1 tile → starts exiting pipe
TC_AI_009: PiranhaPlant → Mario next to pipe opening → does NOT exit
TC_AI_010: HammerBro → throwTimer expires → throws Hammer (Hammer entity created)
TC_AI_011: Lakitu → throwTimer expires → creates Spiny (falls to ground)
TC_AI_012: BulletBill → always flies toward Mario on spawn
```

### test_spatial_hash.cpp

```
TC_SPA_001: Insert entity → query entity's cell → returns that entity
TC_SPA_002: Query empty cell → returns empty list
TC_SPA_003: Entity moves to different cell → hash updated → query correct
TC_SPA_004: Query entity larger than 1 cell → returns all relevant cells correctly
TC_SPA_005: Performance test: 500 entities, query time < 1ms
```

---

## 8.2 Extended Edge Cases — Complete Classification

### 8.2.1 Physics Edge Cases

```
PHYS_EDGE_001: Lag spike of 500ms
  Description : Machine suddenly lags → frameTime = 500ms
  Problem     : accumulator += 0.5s → physics loop runs 30 times
                → "death spiral" — CPU 100%, game freezes
  Fix         : frameTime = min(frameTime, 0.25f) → max 15 physics steps/frame

PHYS_EDGE_002: Mario's velocity.y extremely high, tunnels through floor
  Description : High fall speed in 1 frame → Mario passes through thin tile
  Problem     : "Tunneling" — collision not detected
  Fix         : Cap velocity.y at MAX_FALL_SPEED or use swept AABB

PHYS_EDGE_003: Mario standing on fast-moving platform
  Description : Platform moves quickly downward → Mario floats or clips in
  Fix         : Mario inherits platform velocity when grounded

PHYS_EDGE_004: Mario jumps into ceiling at exact moment
  Description : Ceiling collision while RISING → velocity.y set to 0
  Expected    : Mario stops at head, falls immediately

PHYS_EDGE_005: Gravity scale = 0 (underwater swimming)
  Description : Underwater → gravity_scale = 0.3
  Verify      : Mario does not fall fast, can swim upward multiple times
```

### 8.2.2 Collision Edge Cases

```
COL_EDGE_001: Mario standing exactly at tile edge (1px outside)
  Description : Mario position.x + width = tile.x → no overlap
  Expected    : Mario is NOT considered standing on tile → falls
  Risk        : Off-by-one error → Mario hangs in mid-air

COL_EDGE_002: Two adjacent tiles creating a seam gap
  Description : Tile A and Tile B adjacent but with pixel gap
  Problem     : Mario gets stuck in gap → "micro-stuck"
  Fix         : Tile map has no gaps (integer positions)

COL_EDGE_003: Mario jumps into tile corner
  Description : Mario flies up → lower-right corner hits upper-left of tile
  Expected    : Corner correction → Mario passes through
  Fix         : If overlap.x < 3px AND Mario is RISING → nudge sideways, don't block

COL_EDGE_004: Same frame — Mario kills enemy and enemy touches Mario
  Description : Frame N: Mario falls on enemy head + enemy touches Mario body simultaneously
  Expected    : If Mario stomps from above → enemy dies, Mario is safe
  Fix         : Priority: stomp detection (top hit) > side hit damage

COL_EDGE_005: Two sliding shells collide
  Description : Shell #1 flies right, Shell #2 flies left, they meet
  Expected    : Both shells stop and/or are destroyed
  Fix         : Shell vs Shell → both destroy, points awarded

COL_EDGE_006: Shell bounces off wall back into Mario
  Description : Mario kicks shell → shell flies → hits wall → bounces back → kills Mario
  Expected    : Mario takes damage/dies (intentional mechanic in original game)
  Fix         : Sliding shell also hurts Mario on side contact

COL_EDGE_007: Fireball and Mario stomp Goomba on same frame
  Description : Fireball and Mario stomp same Goomba in same frame
  Expected    : Goomba dies, score = fireball_score OR stomp_score (not both)
  Fix         : Goomba dies only once, ENEMY_KILLED event published only once

COL_EDGE_008: Mario enters pipe exactly when timer expires
  Description : Mario is pipe-transitioning when timer hits 0
  Expected    : Level transition takes priority, not timer death
  Fix         : If mario.isPipeTransitioning → ignore timer death
```

### 8.2.3 Enemy Behavior Edge Cases

```
ENEMY_EDGE_001: 2 Goombas collide with each other
  Description : Goomba #1 and #2 walk toward each other → collision
  Expected    : Both reverse direction (bounce off)
  Fix         : Enemy vs Enemy collision → velocity.x sign reversed

ENEMY_EDGE_002: Red KoopaTroopa at exact cliff edge
  Description : Red Koopa reaches edge → detects void → reverses
  Problem     : If tile check is wrong → Koopa falls (bug)
  Fix         : Check tile at (x + step, y + 1) each frame

ENEMY_EDGE_003: Sliding shell and Mario standing right next to wall
  Description : Mario kicks shell → shell flies 1 tile → meets Mario at opposite wall
  Expected    : No "teleporting" shell, Mario gets hit at correct timing

ENEMY_EDGE_004: Piranha Plant when Mario stands at pipe opening
  Description : Mario stands on pipe top → Piranha does not come out
  Threshold   : Mario within 1 tile of pipe opening → stays hidden
  Edge case   : Mario exactly 1 tile away → Piranha can appear

ENEMY_EDGE_005: Lakitu dies while Spiny is still airborne
  Description : Mario shoots Fireball → Lakitu dies → Spiny is mid-air
  Expected    : Spiny continues falling, operates normally
  Fix         : Spiny is an independent entity once created

ENEMY_EDGE_006: Bowser moves off-screen
  Description : Bowser moves outside camera view
  Fix         : Bowser position clamped within boss arena bounds

ENEMY_EDGE_007: HammerBro throws hammer and Mario stomps it simultaneously
  Description : Hammer is mid-air + Mario stomps HammerBro at same time
  Expected    : HammerBro dies, hammer continues flying, can still hit Mario

ENEMY_EDGE_008: BulletBill stomped while on screen
  Description : BulletBill flies → Mario jumps down from above → stomp
  Expected    : BulletBill dies + score + Mario bounces slightly
```

### 8.2.4 Item & Block Edge Cases

```
ITEM_EDGE_001: Mushroom falls into void immediately after spawning
  Description : Block at cliff edge → Mushroom spawns → falls before Mario picks up
  Fix         : Mushroom has physics → falls, lost if it goes into void

ITEM_EDGE_002: Star bounces off-screen (bounces infinitely upward)
  Description : Star spawns → bounces up → exceeds ceiling → disappears
  Fix         : Star limited by ceiling collision

ITEM_EDGE_003: FireFlower when Mario is Small
  Description : In original NES: Block contains FireFlower + Small Mario → spawns Mushroom instead
  Fix         : EntityFactory checks MarioState when spawning item

ITEM_EDGE_004: Question Block hit multiple times
  Description : Mario hits block → item comes out → Mario hits again
  Expected    : Block switches to EMPTY_BLOCK → only "block hit" sound, no item

ITEM_EDGE_005: Coin Block (multiple coins)
  Description : Block holds many coins → player holds jump into block repeatedly
  Fix         : coinCount-- each hit, when coinCount=0 → EMPTY_BLOCK

ITEM_EDGE_006: Hidden Block
  Description : Invisible block → Mario jumps into exact position
  Expected    : Block appears (light/puff effect) → works like Question Block

ITEM_EDGE_007: 1-Up Mushroom falls into void before collected
  Description : 1-Up from hidden block → falls into void
  Expected    : 1-Up lost, Mario does not receive +1 life
```

### 8.2.5 Level & Camera Edge Cases

```
LEVEL_EDGE_001: Camera must not scroll backward
  Description : Mario runs right → camera.x increases → Mario turns left
  Expected    : Camera.x does NOT decrease, Mario constrained to not go behind camera edge

LEVEL_EDGE_002: Mario at end of map (flagpole area)
  Description : camera.x at maximum → no more scrolling, Mario runs to flag
  Fix         : Camera clamped to map boundary

LEVEL_EDGE_003: Sub-area (pipe exit) has its own camera
  Description : Mario enters pipe to underground → camera resets to (0,0) in sub-area
  Fix         : Sub-area is a separate TileMap, with its own Camera

LEVEL_EDGE_004: Timer = 0 exactly when Mario grabs flagpole
  Description : Mario touches flag pole exactly when timer hits 0
  Expected    : Level complete takes priority over timer death
  Fix         : If mario.touchedFlagPole → ignore timer

LEVEL_EDGE_005: Mario dies during pipe transition
  Description : Bowser fire hits Mario while Mario is entering pipe
  Fix         : PipeTransition state → ignore damage (invincible while transitioning)

LEVEL_EDGE_006: World 8-4 loop detection
  Description : Wrong path taken → loops back to section start (not level start)
  Fix         : Checkpoint flags at each branch point
```

### 8.2.6 Audio Edge Cases

```
AUDIO_EDGE_001: Starman music ends → resume correct BGM type
  Description : Star expires → must resume correct BGM (overworld/underground/castle)
  Fix         : AudioManager stores previousBGM before playing starman theme

AUDIO_EDGE_002: Multiple SFX simultaneously
  Description : Mario stomps 3 Goombas + collects coin + jumps in 1 frame
  Fix         : SDL2_mixer with 8 channels → mix simultaneously, no sound loss

AUDIO_EDGE_003: Audio device unavailable
  Description : Machine has no sound card or driver error
  Fix         : Game continues running without audio, logs warning
```

### 8.2.7 Score & Lives Edge Cases

```
SCORE_EDGE_001: Score exceeds 9,999,999
  Fix         : uint32_t cap, display "9999999" (no rollover)

SCORE_EDGE_002: Exactly 100 coins at count 99 → 100
  Fix         : Event COIN_COUNT_CHANGED → check == 100 → +1 life, reset = 0

SCORE_EDGE_003: Lives = 0 and Mario dies
  Fix         : Publish GAME_OVER, do NOT publish MARIO_DIED again

SCORE_EDGE_004: Consecutive stomp combo — Mario bounces immediately
  Description : Mario stomps 4 Goombas consecutively without touching ground
  Expected    : Combo 100→200→400→800, bounces up after each stomp

SCORE_EDGE_005: Stomping last enemy in combo → 1-UP
  Description : 9 consecutive stomps in combo → 9th and beyond → +1 Life
  Fix         : comboCount >= 9 → +1 life instead of score
```

---

## 8.3 Performance & Stress Tests

```
STRESS_001: 200 Entities simultaneously
  Setup    : Spawn 200 Goombas in 1 level
  Expected : FPS >= 30 (target 60)
  Metric   : profileTime(collisionSystem.update) < 8ms

STRESS_002: 500 Particles simultaneously
  Setup    : Trigger particle effects continuously
  Expected : FPS drops no more than 10% compared to no particles
  Metric   : ObjectPool works, no new heap allocations

STRESS_003: Very large map (500 × 15 tiles)
  Setup    : Load 500-tile-wide map
  Expected : Only renders visible tiles (effective culling)
  Metric   : renderSystem.render() time < 2ms

STRESS_004: Large asset loading
  Setup    : Load all sprite sheets and audio simultaneously
  Expected : AssetManager caches on first load, subsequent accesses < 1ms
  Metric   : No re-loading of already cached assets

STRESS_005: Save/Load 100 times consecutively
  Setup    : Save and load game state 100 times in a loop
  Expected : No memory leaks, file never corrupted
  Metric   : save.json size stable, < 10KB
```

---

# PART IX — AGILE ROADMAP (5 PHASES)

---

## Gantt Chart

```
                  WEEK
           1  2  3  4  5  6  7  8  9
Phase 1:  [████████]
Phase 2:        [████████]
Phase 3:              [████████]
Phase 4:                    [████████]
Phase 5:                          [████]
```

---

## PHASE 1 — MVP "Core Engine" (Weeks 1-2)

**Goal:** Mario can move, jump, and collide with simple tiles.  
**Not needed yet:** Sprites, audio, enemies, items.

| # | Task | File Created | Done When |
|---|---|---|---|
| 1.01 | Setup CMakeLists.txt + SDL2 FetchContent | CMakeLists.txt | `cmake --build` succeeds |
| 1.02 | `Constants.h` — define all game constants | Constants.h | Compiles clean |
| 1.03 | `Vector2D.h` + `Rect.h` + `MathUtils.h` | utils/ | Unit tests pass |
| 1.04 | `Window` class — SDL_Window + SDL_Renderer | Window.h/cpp | 800×600 window appears |
| 1.05 | `Logger` Singleton — file + console output | Logger.h/cpp | game.log is written |
| 1.06 | `Entity` abstract base class | Entity.h/cpp | Compiles clean |
| 1.07 | `PhysicsComponent`, `ColliderComponent` | components/ | Compiles clean |
| 1.08 | `PhysicsSystem` — gravity + velocity update | PhysicsSystem.h/cpp | Mario falls down |
| 1.09 | `InputSystem` — SDL poll, InputState struct | InputSystem.h/cpp | Key state correct |
| 1.10 | `Mario` class — placeholder rectangle | mario/Mario.h/cpp | Red rectangle visible |
| 1.11 | Mario movement + acceleration + deceleration | Mario.cpp | Movement feels smooth |
| 1.12 | `TileMap` — hardcoded 2D array, solid tiles | TileMap.h/cpp | Ground floor appears |
| 1.13 | `CollisionSystem` — Mario vs Tile AABB | CollisionSystem.h/cpp | Mario does not pass through floor |
| 1.14 | Jump: variable height + Coyote Time + Buffer | Mario.cpp | Can jump high or low |
| 1.15 | `GameLoop` — Fixed Timestep | GameLoop.h/cpp | FPS counter ~60 |
| 1.16 | `Game` class — initialization, cleanup | Game.h/cpp | Game runs without crash |
| 1.17 | `IGameState` interface + basic `PlayState` | states/ | State machine works |
| 1.18 | Debug overlay — hitbox, FPS (F1 toggle) | RenderSystem.cpp | Debug can be toggled |

**Acceptance Criteria:**
- Mario moves left/right, can jump
- Does not pass through the floor
- Gravity feels weighted and realistic
- FPS stable at ~60

---

## PHASE 2 — WORLD "Real Level, First Enemy" (Weeks 3-4)

| # | Task | File Created | Done When |
|---|---|---|---|
| 2.01 | `AssetManager` Singleton — load PNG, WAV | AssetManager.h/cpp | Texture cache works |
| 2.02 | `AnimationComponent` — sprite sheet frames | AnimationComponent.h | Animation runs |
| 2.03 | Mario sprite + walk/jump/idle animation | mario/Mario.cpp | Mario has real appearance |
| 2.04 | `Camera` — follows Mario, clamped boundary | Camera.h/cpp | Screen scrolls |
| 2.05 | `LevelLoader` — JSON → TileMap | LevelLoader.h/cpp | world_1_1.json loads |
| 2.06 | Correct tile rendering (overworld tileset) | TileMap.cpp | Level looks good |
| 2.07 | `EventBus` Singleton | EventBus.h/cpp | Pub/sub works |
| 2.08 | `MarioFSM` — SMALL/SUPER states | MarioFSM.h/cpp | State transitions work |
| 2.09 | `EntityFactory` — create entities from map data | EntityFactory.h/cpp | Goomba appears at correct position |
| 2.10 | `Goomba` — basic AI, onStomped() | Goomba.h/cpp | Stomp → flattens |
| 2.11 | `QuestionBlock` — hit from below → spawn item | QuestionBlock.h/cpp | Block releases item |
| 2.12 | `Mushroom` — movement, onPickup() | Mushroom.h/cpp | Small → Super |
| 2.13 | `ScoreSystem` — listens to Events | ScoreSystem.h/cpp | Score increases correctly |
| 2.14 | `HUD` — Score, Coin, World, Timer, Lives | HUD.h/cpp | HUD displays correctly |
| 2.15 | `AudioManager` + SDL2_mixer setup | AudioManager.h/cpp | BGM plays |
| 2.16 | SFX: jump, stomp, coin, powerup | AudioManager.cpp | Sounds play correctly |
| 2.17 | Timer countdown + TIMER_WARNING event | PlayState.cpp | Music speeds up <100s |
| 2.18 | Flag Pole — level end | TileMap.cpp | Level Clear |
| 2.19 | `TransitionState` — scene transition | TransitionState.h/cpp | Transition looks good |
| 2.20 | `Coin` entity — collection + score | TileMap.cpp | Coin count is correct |

**Acceptance Criteria:**
- Can play World 1-1 from start to flag
- Goomba works correctly
- Pick up Mushroom → grow bigger
- Audio + HUD are correct

---

## PHASE 3 — POLISH "True Game Feel" (Weeks 5-6)

| # | Task | File Created | Done When |
|---|---|---|---|
| 3.01 | Breakable bricks + break particles | BrickBlock.h/cpp | Breaks correctly |
| 3.02 | `KoopaTroopa` full shell mechanic | KoopaTroopa.h/cpp | Shell slides |
| 3.03 | `KoopaShell` — separate entity | KoopaShell.h/cpp | Shell kills enemies |
| 3.04 | `FireFlower` + `MarioFSM::FIRE` | FireFlower.h/cpp | Mario can shoot |
| 3.05 | `Fireball` — bounce physics | Fireball.h/cpp | Bounces correctly |
| 3.06 | Fireball max 2 at once | Mario.cpp | Limit enforced |
| 3.07 | `StarPower` — invincible + flashing sprite | StarPower.h/cpp | Star lasts 10s |
| 3.08 | `OneUpMushroom` | OneUpMushroom.h/cpp | +1 life |
| 3.09 | Pipe mechanics — enter/exit | TileMap.cpp | Pipe transition works |
| 3.10 | `PiranhaPlant` — hide/show logic | PiranhaPlant.h/cpp | Hides when Mario is near |
| 3.11 | `BulletBill` + `BulletBillLauncher` | BulletBill.h/cpp | Flies correctly |
| 3.12 | `ParticleSystem` — SCORE_POPUP, DUST | ParticleSystem.h/cpp | Particles appear |
| 3.13 | Stomp combo score (100→200→...→1UP) | ScoreSystem.cpp | Combo is correct |
| 3.14 | Skid effect — sprite + dust particle | Mario.cpp | Slides when reversing direction |
| 3.15 | Corner correction for jumps | CollisionSystem.cpp | No more corner-sticking |
| 3.16 | World 1-1, 1-2, 1-3 maps complete | maps/ | 3 playable levels |
| 3.17 | Mario death animation + respawn | Mario.cpp | Death works correctly |
| 3.18 | Invulnerability flash after getting hit | Mario.cpp | Flashes for 2s |

**Acceptance Criteria:**
- Someone who doesn't know it's a clone → feels exactly like original Mario
- All Phase 1-2-3 mechanics work correctly

---

## PHASE 4 — ADVANCED "Complete Project" (Weeks 7-8)

| # | Task | File Created | Done When |
|---|---|---|---|
| 4.01 | Full `SceneManager` stack | SceneManager.h/cpp | Menu/Pause/GameOver |
| 4.02 | `MenuState` — Title Screen | MenuState.h/cpp | Menu looks good |
| 4.03 | `PauseState` — overlay | PauseState.h/cpp | Pause does not reset |
| 4.04 | `GameOverState` | GameOverState.h/cpp | Game over screen |
| 4.05 | `HammerBro` — hammer-throwing AI | HammerBro.h/cpp | Hammer flies correctly |
| 4.06 | `Hammer` projectile — arc trajectory | Hammer.h/cpp | Arc physics correct |
| 4.07 | `Lakitu` + `Spiny` | Lakitu.h/cpp | Throws Spiny from above |
| 4.08 | `CheepCheep` (2 variants) | CheepCheep.h/cpp | Swims correctly |
| 4.09 | `Blooper` — underwater AI | Blooper.h/cpp | Chases Mario |
| 4.10 | `FireBar` — rotating fire bar | FireBar.h/cpp | Rotates correctly |
| 4.11 | `FakeBowser` + `Bowser` (real, W8-4) | Bowser.h/cpp | Boss fight works |
| 4.12 | World 1-4 (Castle) complete | maps/ | Boss fight clears |
| 4.13 | World 2-1, 2-2 (Underwater), 2-3, 2-4 | maps/ | World 2 playable |
| 4.14 | World 3, 4, 5 (basic) | maps/ | 16 levels |
| 4.15 | World 6, 7, 8-1→8-4 (final boss) | maps/ | Full game complete |
| 4.16 | Ice physics (World 6) | PhysicsSystem.cpp | Slides on ice |
| 4.17 | Giant World tiles (World 4) | TileMap.cpp | Scaled ×2 |
| 4.18 | Save/Load system — JSON serialize | FileIO.h/cpp | Progress saved |
| 4.19 | Highscore — Top 10, file I/O | FileIO.cpp | Leaderboard works |
| 4.20 | `ObjectPool<Fireball>`, `ObjectPool<Particle>` | ObjectPool.h | No heap allocations in loop |
| 4.21 | `SpatialHash` — optimizes CollisionSystem | SpatialHash.h/cpp | 60 FPS with 100+ entities |
| 4.22 | Warp Zones (1-2 secret pipe) | LevelLoader.cpp | Warp works |
| 4.23 | Ending sequence | PlayState.cpp | Credits look good |

**Acceptance Criteria:**
- All 32 levels playable
- Real Bowser in W8-4
- Save/Load works
- 60 FPS with 50+ entities

---

## PHASE 5 — RELEASE "Packaging" (Week 9)

| # | Task | Done When |
|---|---|---|
| 5.01 | CMake Release build (-O2 optimization) | Build size < 5MB |
| 5.02 | Package exe + DLL + assets (Windows) | Runs on clean machine |
| 5.03 | Doxygen — HTML API docs | docs/ complete |
| 5.04 | Google Test — unit test coverage ≥ 80% | Test report generated |
| 5.05 | Code review — remove magic numbers, hardcodes | No mysterious "100" literals remain |
| 5.06 | README.md — Build, Run, Controls, Credits | Other devs can clone → build immediately |
| 5.07 | Performance profiling — CPU/Memory report | No bottlenecks identified |
| 5.08 | Demo video/GIF gameplay | Visual documentation complete |

---

# PART X — TECH STACK & DEPENDENCIES

---

## 10.1 Complete Tech Stack

| Component | Technology | Version | License | Reason |
|---|---|---|---|---|
| Language | **C++17** | C++17 | — | `unique_ptr`, `optional`, `variant`, `filesystem` |
| Build | **CMake** | 3.20+ | BSD | Cross-platform, FetchContent auto-downloads deps |
| Window/Render | **SDL2** | 2.26+ | zlib | Cross-platform, lightweight, indie game standard |
| Image Loading | **SDL2_image** | 2.6+ | zlib | PNG/JPG support |
| Audio | **SDL2_mixer** | 2.6+ | zlib | Mix BGM + 8 SFX channels |
| JSON | **nlohmann/json** | 3.11+ | MIT | Header-only, easy to use, fast parsing |
| Unit Testing | **Google Test** | 1.14+ | BSD | Most popular C++ test framework, good CMake integration |
| Documentation | **Doxygen** | 1.9+ | GPL | Auto-generates API docs from comments |
| Package Mgr | **CMake FetchContent** | built-in | — | Auto-downloads SDL2, GTest — no manual install |

## 10.2 CMakeLists.txt Skeleton

```cmake
cmake_minimum_required(VERSION 3.20)
project(SuperMario VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Auto-download dependencies
include(FetchContent)

FetchContent_Declare(SDL2
  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
  GIT_TAG release-2.26.5)
FetchContent_MakeAvailable(SDL2)

FetchContent_Declare(SDL2_image
  GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
  GIT_TAG release-2.6.3)
FetchContent_MakeAvailable(SDL2_image)

FetchContent_Declare(SDL2_mixer
  GIT_REPOSITORY https://github.com/libsdl-org/SDL_mixer.git
  GIT_TAG release-2.6.3)
FetchContent_MakeAvailable(SDL2_mixer)

FetchContent_Declare(json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.3)
FetchContent_MakeAvailable(json)

FetchContent_Declare(googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.14.0)
FetchContent_MakeAvailable(googletest)

# Source files
file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(super_mario ${SOURCES})

target_include_directories(super_mario PRIVATE include)
target_link_libraries(super_mario PRIVATE
  SDL2::SDL2 SDL2_image::SDL2_image SDL2_mixer::SDL2_mixer
  nlohmann_json::nlohmann_json)

# Tests
enable_testing()
add_subdirectory(tests)
```

---

# 🚨 CHECKPOINT — AWAITING APPROVAL

> **The analysis and implementation plan report is complete.**  
>  
> This document covers:  
> ✅ Full original game analysis (mechanics, physics, enemies, worlds)  
> ✅ Detailed Input/Output specification  
> ✅ 4-layer system architecture + full directory tree  
> ✅ Fixed Timestep Game Loop — step-by-step logic  
> ✅ Complete list of 30+ Entities with class hierarchy  
> ✅ Detailed structure of all 8 Worlds  
> ✅ 15 risks + mitigation strategies  
> ✅ 90+ Test Cases + 50+ Edge Cases fully classified  
> ✅ Agile 5-Phase roadmap — 9 weeks  
> ✅ Tech stack + CMake skeleton  
>  
> **NO CODE HAS BEEN WRITTEN YET.**

| Command | Action |
|---|---|
| `BUILD PHASE 1` | Start implementing Phase 1 — Core Engine |
| `ADJUST [section]` | Change architecture or design decisions |
| `MORE DETAIL [section]` | Add additional information for a specific section |

---
