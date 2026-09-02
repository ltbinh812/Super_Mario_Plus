# SUPER MARIO PLUS — MAJOR FEATURE LIST

**Course:** CS202 – Programming Systems · **Class:** 25A01 · **Group:** 51
| 25125028 Phạm Đức Minh | 25125007 Lê Tiến Bình |

---

## How this list is scoped

**39 major features.** One whole system = one line, at the granularity of the grading sheet
("AI", "MultiplePlayers", "Save Game", "map building"). Individual content is **not** counted: one
coin, one enemy type or one room is not a feature. Where a system contains many pieces the count goes
in the note — "9 enemy types" is one feature, not nine.

**Rows 1–9 are the topics named in the brief**, in the order given. Rows 10–39 are the remaining
systems.

`SuperMarioPlus_Feature_Catalog.md` breaks these systems down and explains each mechanism in detail;
this sheet is the summary for grading. Point columns follow the reference sheet's convention (5 for
the headline topics, 2.5 otherwise) — reconcile against the official scheme before submitting.

---

## A. Required topics

| # | Feature | Max | Group | Note | Key files |
|---|---|---|---|---|---|
| 1 | **PlayerInputsMovementCollision** | 5 | 5 | Rebindable input for two players on one keyboard, routed as command objects so no gameplay code holds a key constant. Ten movement behaviours — run, jump, fall, crouch, dash, block, climb, swim, plus hurt and death. Axis-separated AABB resolution against a 13-value collision grid, with one-way platforms, drop-through, and slopes walked smoothly. | `InputHandler.cpp` · `PlayerCommands.h` · `entity/Player/Player*State.cpp` · `Entity.cpp` |
| 2 | **EnemyBehavior** | 5 | 5 | Nine enemy types from one `Mob` class + JSON. Seven-state FSM (idle, patrol, chase, attack, skill, hurt, die) with vision range, attack range, patrol timing and attack cooldown all per type. Three attack archetypes: melee, projectile, explosion. | `entity/EnemyStates/` (7) · `Mob.cpp` · `enemies.json` |
| 3 | **PowerUpsItems** | 5 | 5 | Thirteen interactable types (coins, keys, doors, lucky blocks, two chests, four spring orientations, flags, bombs, poison flasks, buff pickups, shop, exit gate) and nine buffs — speed, jump, strength, shield, heal, invisibility, gold magnet, time stop, random — that **stack additively** rather than replacing one another. One-slot inventory with swap. | `entity/Item/` (23) · `entity/BuffEffects/` (10) · `BuffManager.cpp` |
| 4 | **LevelCompletion** | 5 | 5 | Sixty-three connected rooms across six worlds. Progress gated by keys and locked doors, secured at checkpoint flags, finished at an animated exit gate that opens the completion screen. Room-to-room movement preserves everything already done. | `BaseLevelState.cpp` · `Flag.cpp` · `EndgameAsset.cpp` · `EndgameState.cpp` |
| 5 | **Sounds** | 5 | 5 | Five independent channels — master, menu music, per-map background, player SFX, enemy SFX. Streaming music vs. cached effects. Enemy sound attenuates with distance. **Footsteps bound to animation keyframes**, so they land exactly on the frames where feet plant. | `AudioManager.cpp` · `AssetManager.cpp` · `Player.cpp` · `Mob.cpp` |
| 6 | **OOD** | 5 | 5 | Nine abstract interfaces as the extension points; composition over inheritance (`Player` owns states, skills, buffs, configs rather than subclassing); RAII ownership expressed in the type (`unique_ptr` owns, raw pointers only observe); one class per file across 328 files; SOLID applied concretely — see report §4. | `entity/Entity.h` · `IEntityState.h` · `IMobState.h` · `ISkill.h` |
| 7 | **DesignPatterns** | 5 | 5 | State (two variants — by value for the player, by pointer for mobs), Strategy (six applications), Command (two pipelines), Simple Factory (five), Singleton, Memento, Flyweight, Repository, Facade, DTO, Registry, Template Method, producer–consumer queue. Four patterns deliberately rejected with reasons — see report §5. | Report §5 · `command/` · `save/` · `UndoRedoStack.cpp` |
| 8 | **AI** | 5 | 5 | Enemy FSM (7 states) and boss FSM (9 states) driven from JSON. Enemies **jump over obstacles** and **refuse to walk off cliffs** or into lava — both read from one `onHitWall(isCliff)` callback. Nearest-living-target selection that skips invisible players. Bosses add a cutscene-gated intro and a one-time **teleport** entrance when a player first comes within five blocks. | `entity/EnemyStates/` · `entity/BossStates/` (9) · `Boss.cpp` |
| 9 | **MultiplePlayers** | 5 | 5 | Two players on one keyboard with fully independent key maps, health, mana, breath and item slots. Shared camera frames both; co-op pools coins and keys; enemies retarget to whoever is nearer. Versus mode lets them damage each other. | `BaseLevelState.cpp` · `MapCamera.cpp` · `EntityFaction.h` |

## B. Other major features

| # | Feature | Max | Group | Note | Key files |
|---|---|---|---|---|---|
| 10 | **Map building (in-game level editor)** | 2.5 | 2.5 | Editor reachable from the menu: pannable/zoomable grid canvas, categorised block palette drawn from five tilesets, entity palette including **all nine enemies and all six bosses**, spawn-point placement, grid placement and eraser on right-click, translucent placement preview. | `MapEditorState.cpp` · `editor/` (25 headers) |
| 11 | **Rule-based auto-tiling** | 2.5 | 2.5 | Paint terrain and the correct edges, corners and randomised variants are chosen automatically using LDtk's own rule-group format — editor output is visually indistinguishable from the authored campaign maps. | `AutoTiler.cpp` · `extracted_rules.json` |
| 12 | **Undo/redo + live map resize** | 2.5 | 2.5 | Fifty-step snapshot history covering every edit including resizes; drag handles on all four edges grow or shrink the map with existing content preserved. | `UndoRedoStack.cpp` · `EditorMapResizer.cpp` |
| 13 | **Custom map save/load + instant test play** | 2.5 | 2.5 | Maps save to numbered JSON slots, reopen for editing, and launch as **real playable levels** with working enemies, bosses, items and checkpoints — validated first for spawn point and exit. | `CustomMapSerializer.cpp` · `CustomMapValidator.cpp` |
| 14 | **Save game (multi-version)** | 2.5 | 2.5 | Every checkpoint writes a **new numbered version** rather than overwriting, per world, so a run keeps its full history and the player can return to any point. Four-layer stack (DTO → serializer → repository → facade) with atomic writes. | `save/` (10 files) · `SaveManager.cpp` |
| 15 | **Checkpoint & world-state restore** | 2.5 | 2.5 | Dying rewinds the **world**, not just the player: enemies killed after the checkpoint come back, chests opened after it close again. Built on LDtk's stable entity identifiers. | `BaseLevelState.cpp` · `LevelSaveData.h` |
| 16 | **Screen flow & scene management** | 2.5 | 2.5 | A screen stack routes menu → world selection → new-game/load panel → character selection → loading → level → endgame, plus the editor and settings, with iris transitions between. Screens can layer (pause over a running level) and replace themselves safely through deferred commands. | `StateManager.cpp` · `states/` (17) · `StateCommands.h` |
| 17 | **Key rebinding & settings persistence** | 2.5 | 2.5 | All eleven actions rebindable independently for P1 and P2, from the main menu **or** mid-game. Changes apply on the next frame without leaving the level. Bindings and all five volumes persist to `saves/settings.json`, at a fixed path that does not depend on how the game was launched. | `SettingsManager.cpp` · `ButtonGroup.cpp` · `AppPaths.cpp` |
| 18 | **Multiple game modes** | 2.5 | 2.5 | Single-player story campaign, two-player co-op story, two-player PvP across six dedicated arenas, and custom-map play in either one- or two-player form. | `BaseLevelState.cpp` · `World01–06State.cpp` |
| 19 | **6 playable characters, data-driven** | 2.5 | 2.5 | Goku, Naruto, Luffy, Kakashi, Sasuke, Zoro — different health, mana, speed, jump, hitbox, 17–21 animations, own projectiles and explosions. All of it declared in JSON, so a seventh character is a config block plus sprites, with no new class. | `characters.json` · `PlayerFactory.cpp` |
| 20 | **Skill system (10 per character)** | 2.5 | 2.5 | Four combo attacks, jump attack, low attack, ranged projectile, special, dash and block — all behind one `ISkill` interface, each with its own mana cost, timing, hitbox and animation. | `entity/Skill/` (15 headers) |
| 21 | **Combo chain with timing windows** | 2.5 | 2.5 | Four-hit chain where the follow-up only fires inside a timing window; each attack **names its own successor in data**, so the chain is configuration rather than nested conditionals. Hit windows are authored in animation frames and converted to seconds at load, so damage and artwork always line up. | `ISkill.h` · `PlayerSkillState.cpp` · `PlayerFactory.cpp` |
| 22 | **Central combat system** | 2.5 | 2.5 | All damage resolved in one place per simulation step through a pluggable collision detector: faction masks decide who may hit whom, defence subtraction, directional knockback, invincibility frames, explosion AoE. | `CombatSystem.cpp` · `Hitbox.h` · `Explosion.cpp` |
| 23 | **Hit-stop (combat feel)** | 2.5 | 2.5 | On a connecting hit the attacker's animation and skill timer freeze for a few hundredths of a second, giving blows weight. Duration is per skill from JSON; a miss does not trigger it. | `Player.cpp` · `CombatSystem.cpp` |
| 24 | **Player movement state machine** | 2.5 | 2.5 | Ten behaviours as objects with their own transitions and an exit guard, so a cancelled attack structurally cannot keep dealing damage and a crouching player cannot be walked out of the crouch. | `entity/Player/Player*State.cpp` |
| 25 | **Vitals: health, i-frames, hurt & respawn** | 2.5 | 2.5 | Per-character health pool, hurt state that interrupts the current action, one second of invincibility with a visible flash, a death animation that must finish before respawn, and respawn to the last checkpoint. | `Player.cpp` · `PlayerHurtState.cpp` · `PlayerDieState.cpp` |
| 26 | **Mana economy** | 2.5 | 2.5 | Skills consume mana, mana regenerates in every state, and attempting a skill without enough shows a message rather than failing silently. Costs are per skill, from JSON. | `Player.cpp` · `ISkill.h` |
| 27 | **Swimming & breath** | 2.5 | 2.5 | Water replaces the movement model entirely — four-way swimming, reduced gravity, drag — and drains an oxygen bar that causes periodic drowning damage at zero, refilling on surfacing. | `PlayerSwimState.cpp` · `Player.cpp` |
| 28 | **Terrain & environmental interaction** | 2.5 | 2.5 | Thirteen terrain behaviours, and they act on **every entity, not just the player**: lava and poison apply damage-over-time to enemies as well; water makes anything in it float and move slowly; clouds are passed through when moving fast enough; lotus pads float; slopes are walked smoothly; instant-death tiles kill on contact. | `TileMap.h` · `Entity.cpp` · `Effects.cpp` |
| 29 | **9 enemy types** | 2.5 | 2.5 | Rat, bat, goblin, slime, mushroom, skeleton, soldier, tree, guardian — health 30–200, speed 50–150, detection 100–400, attack range 35–150, each with its own animation set and skill list. | `enemies.json` · `EnemyFactory.cpp` |
| 30 | **6 boss encounters** | 2.5 | 2.5 | Doflam, Franky, Itachi, Sasuke, Shank, Naruto — 1000 HP set pieces inheriting all mob movement and sensing, plus nine states, four combo attacks, explosion and long-range attacks, and a cutscene gate so the fight cannot start under the dialogue. | `Boss.cpp` · `BossIntroState.cpp` · `entity/BossStates/` |
| 31 | **LDtk level pipeline** | 2.5 | 2.5 | Levels authored in the LDtk editor and loaded **directly** from the `.ldtk` project — tile layers, collision grid, entity placements with custom fields, and the room graph. No export step, so what the designer saves is what the game loads. | `TileMap.cpp` · `assets/maps/` |
| 32 | **Seamless room transitions** | 2.5 | 2.5 | Walking off a room edge loads the neighbouring room with no loading screen and places the player correctly on the far side, preserving defeated enemies and changed items. | `BaseLevelState.cpp` · `TileMap.cpp` |
| 33 | **Camera system** | 2.5 | 2.5 | Dead-zone smooth follow with velocity look-ahead and map clamping, two-player framing that keeps both on screen, and queued cinematic pan/zoom for cutscenes — returning to normal is a queued item, so an aborted cutscene still recovers. | `MapCamera.cpp` · `CameraFollowMode.cpp` · `CameraPanMode.cpp` |
| 34 | **Cutscene & dialogue system** | 2.5 | 2.5 | Trigger volumes authored in LDtk take control away, move the camera, and run a typewriter conversation showing the speaker's name **and character portrait**, then hand control back. One-shot per trigger. | `CutsceneManager.cpp` · `DialogueBox.cpp` · `DialogueRegistry.cpp` |
| 35 | **In-game shop & economy** | 2.5 | 2.5 | A shop building placed in the world opens a scrollable catalogue with artwork and prices; buying deducts from the shared coin pool and reports insufficient funds. The world pauses while it is open. | `ShopAsset.cpp` · `ShopUIPanel.cpp` |
| 36 | **Resolution-independent interface** | 2.5 | 2.5 | Every screen laid out against a virtual 2560×1440 canvas and scaled uniformly with letterboxing, so proportions are identical on any monitor and artwork is never distorted. | `UIScaler.cpp` |
| 37 | **HUD & visual feedback** | 2.5 | 2.5 | Portrait, health/mana/breath bars, coin and key totals, held item and active buff timers — per player in two-player mode. Plus floating damage numbers, poison and burning overlays, buff auras, curved projectile trails, real animated-GIF backgrounds and objects, and iris screen transitions. | `PlayerHUD.cpp` · `GifAnimation.cpp` · `IrisTransition.cpp` · `Effects.cpp` |
| 38 | **Loot drops** | 2.5 | 2.5 | Every enemy drops a coin when it dies; **every boss always drops a key** — the item that opens the next door or gate. That ties campaign progress to defeating the boss, and closes the economy loop: coins come from combat, not only from the map. | `EnemyDieState.cpp` · `BossDieState.cpp` · `ItemFactory.cpp` |
| 39 | **Screen shake on impact** | 2.5 | 2.5 | A landed hit shakes the view with a decaying random offset. Applied as an additive offset at draw time rather than a camera mode, so the camera keeps following the player while shaking; a miss does not trigger it. | `MapCamera.cpp` · `BaseLevelState.cpp` |

---

## Content shipped (not counted as features)

| Item | Count |
|---|---|
| Campaign worlds | 6 |
| Campaign rooms | 63 |
| PvP arenas | 6 |
| Playable characters | 6 |
| Enemy types | 9 |
| Boss encounters | 6 |
| Buff types | 9 |
| Item / interactable types | 13 |
| Terrain behaviours | 13 |
| Skills per character | 10 |
| Player behaviour states | 10 |
| Enemy / boss AI states | 7 / 9 |

---

*Group 51 — Phạm Đức Minh (25125028) · Lê Tiến Bình (25125007) — September 2026*
