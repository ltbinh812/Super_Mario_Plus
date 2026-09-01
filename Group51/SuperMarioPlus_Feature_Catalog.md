# SUPER MARIO PLUS — FEATURE CATALOG

**Course:** CS202 – Programming Systems · **Class:** 25A01 · **Group:** 51
University of Science – VNUHCM · Faculty of Information Technology · September 2026

| Student ID | Full name |
|---|---|
| 25125028 | Phạm Đức Minh |
| 25125007 | Lê Tiến Bình |

---

## The number

> # This project implements **99 features**, catalogued below as **F001–F099**.

Separately from that total, the project ships **63 authored campaign rooms** across 6 worlds and
**6 PvP arenas**. Rooms and arenas are listed in Appendix C but are **never counted as features** —
the feature total stands at 99 on its own.

**Counting convention.** A feature is a distinct capability that a player can observe or use, or a
distinct engineering capability that changes what the game is able to do. One entry = one capability.
We deliberately do **not** split one capability into several entries to inflate the count: "run left"
and "run right" are one movement feature, not two; the nine buff types are grouped by what they do
rather than listed as nine separate features. We do not count individual rooms, individual sprites,
individual buttons, or individual configuration values. Capabilities too small to be worth analysing
were dropped rather than padded into the list.

**How to read an entry.** Every feature is a numbered subsection with four parts:

| Part | Contains |
|---|---|
| **Subsystem** | Which part of the architecture owns it |
| **What it does** | The capability, described from the player's side |
| **How it works** | The actual mechanism — data flow, algorithm, ownership |
| **Related files** | The source files that implement it |

Every file named in this document appears in the `set(SOURCES ...)` list of `CMakeLists.txt` or in
`include/`, so every claim can be checked against code that is genuinely compiled into the executable.

---

## Coverage of the required topics

The course brief names a set of topics that a submitted project is expected to demonstrate. Each one
is mapped here to the features that realise it, so a reader can jump straight to the evidence.

| Required topic | Realised by | What is actually there |
|---|---|---|
| **PlayerInputsMovementCollision** | F002, F014–F019, F086 | Four-phase input pipeline, rebindable keys for two players on one keyboard, run / jump / fall / crouch / climb / swim, swept AABB resolution against a 13-value collision grid, one-way platforms, slopes, speed-sensitive clouds |
| **EnemyBehavior** | F039–F045 | Nine enemy types and six bosses, a seven-state enemy FSM and a nine-state boss FSM driven from JSON, three attack archetypes, ledge and hazard avoidance, target selection and attack cooldowns |
| **AI** | F007, F041–F045, F074 | Menu demo AI that plays the game behind the main menu, per-enemy detection and attack ranges, patrol timing, nearest-living-target selection that ignores invisible players, boss phase gating, scripted camera AI |
| **PowerUpsItems** | F046–F058 | Coins, keys, doors, lucky blocks, chests, springs, flags, bombs, poison flasks, a one-slot inventory with swapping, interchangeable use behaviours, and nine buffs whose bonuses stack additively |
| **MultiplePlayers** | F011, F089, F096–F098 | Two-player local co-op on one keyboard with independent bindings, a shared camera that frames both, a pooled party inventory, per-player HUD, a two-pass character picker, and a six-arena PvP mode |
| **map building** | F069–F072, F075–F082 | LDtk-authored campaign maps loaded directly from the `.ldtk` project file, plus a complete in-game editor with a categorised palette, rule-based auto-tiling, undo/redo, live map resizing, JSON save/load and one-key test play |
| **LevelCompletion** | F050, F062–F068, F070, F096–F099 | Checkpoint flags that write versioned saves, room-to-room progression across 63 rooms, an exit gate that ends a run, world-state persistence so completion is not undone by leaving a room, and a completion screen |
| **Sounds** | F006, F087 | A cached sound bank owned by the asset manager and a four-channel volume model — master, music, player SFX, enemy SFX — that persists between sessions |
| **OOD** | F001–F099 (all) | Interface-driven design across nine abstract interfaces, composition over inheritance in `Player`, RAII and `unique_ptr` ownership throughout, and a strict one-class-one-file rule across 328 files |
| **DesignPatterns** | F003, F021, F038, F041, F053, F058, F062, F074, F080, F092 | State (two variants), Strategy (six applications), Command (two pipelines), Simple Factory (five), Singleton, Memento, Flyweight, Repository, Facade, DTO, Registry, Template Method — analysed with reasoning in the design report |

---

## Coverage at a glance

| # | Category | ID range | Count |
|---|---|---|---|
| 1 | Engine and application core | F001–F006 | 6 |
| 2 | Screens and navigation | F007–F013 | 7 |
| 3 | Movement and terrain | F014–F019 | 6 |
| 4 | Combat and skills | F020–F028 | 9 |
| 5 | Survival statistics and status | F029–F034 | 6 |
| 6 | Playable characters | F035–F038 | 4 |
| 7 | Enemies, bosses and AI | F039–F045 | 7 |
| 8 | Items and interactables | F046–F054 | 9 |
| 9 | Buffs and power-ups | F055–F058 | 4 |
| 10 | Shop and economy | F059–F061 | 3 |
| 11 | Save, checkpoint and progression | F062–F068 | 7 |
| 12 | Level, world and camera | F069–F074 | 6 |
| 13 | Map editor | F075–F082 | 8 |
| 14 | Cutscene and dialogue | F083–F085 | 3 |
| 15 | Settings and controls | F086–F088 | 3 |
| 16 | Visual presentation and interface | F089–F095 | 7 |
| 17 | Game modes | F096–F099 | 4 |
| | **TOTAL** | **F001–F099** | **99** |

Identifiers are contiguous: every number from F001 to F099 is an allocated feature, with no gaps and
no duplicates. Appendix A restates the allocation so the arithmetic can be checked directly.

---
---

# 1. Engine and application core

## F001 — Fixed-timestep simulation with frame-spike clamping

**Subsystem:** Engine core

**What it does.** Gameplay advances at exactly the same rate on every machine. A player on a 240 Hz
monitor and a player on a 30 FPS laptop experience identical jump arcs, identical fall speeds and
identical attack timings. A momentary freeze — alt-tabbing away, a level loading, the operating
system stealing the CPU — does not teleport characters through walls when the game resumes.

**How it works.** `Game::runGame` keeps an accumulator. Each iteration it reads the real elapsed time
from raylib, **clamps it to 0.25 s**, and adds it to the accumulator. It then drains the accumulator
in fixed 1/60 s slices, calling `stateManager.Update(fixedDt)` once per slice; leftover time stays in
the accumulator for the next frame. Rendering happens once per real frame and receives
`alpha = accumulator / fixedDt`, the fraction of a step already elapsed, so drawing can interpolate
between simulation steps instead of stuttering. The clamp is the safety valve: without it, a two-second
hitch would queue 120 catch-up steps in a single frame and produce a visible lurch.

**Related files.** `src/core/Game.cpp` · `include/core/Game.h`

---

## F002 — Four-phase frame pipeline

**Subsystem:** Engine core (project-wide contract)

**What it does.** This is the rule that keeps the whole codebase predictable. Reading input,
deciding what to do, changing the world, and drawing the world are four separate phases that never
interleave. The practical benefit is that no object can be half-updated while another object is
already drawing it, and no object can be deleted while something else is still iterating over it.

**How it works.** Every screen implements `HandleInput()`, `Process()`, `Update(float dt)` and
`Render(float alpha) const` from `GameState`; every entity implements the same shape from `Entity`.
`Game::runGame` calls them strictly in that order across the whole state stack. `HandleInput` may only
*record* what was pressed. `Process` acts on those records and services deferred work such as queued
state changes and spawn requests. `Update` is the only phase permitted to mutate simulation state,
and it is the only phase that runs at fixed timestep — it may execute zero, one or several times per
displayed frame. `Render` is `const` and mutates nothing. Widgets follow the same discipline:
`PanelButton::HandleInput` only sets a flag, and `ConsumeClick()` hands it over exactly once during a
later phase.

**Related files.** `include/states/GameState.h` · `include/entity/Entity.h` · `src/core/Game.cpp` · `src/core/StateManager.cpp`

---

## F003 — Screen stack with deferred transitions

**Subsystem:** Engine core

**What it does.** Screens can layer. The pause panel opens on top of a running level and the level is
still there underneath when it closes. The loading screen builds the next screen and hands over to it.
The editor can launch a playable level and get control back afterwards. A screen can also ask to be
replaced by another screen — and it survives long enough to finish the frame it is in.

**How it works.** `StateManager` owns a `std::vector<std::unique_ptr<GameState>>` used as a stack, and
forwards each of the four phases to the top entry (or to several entries where a screen is drawn
through). The dangerous case is a screen that wants to replace *itself*: calling
`stateManager.ChangeState(...)` directly from inside a member function would destroy the object whose
method is still executing. The project solves this by turning transitions into objects. `PushStateCommand`,
`PopStateCommand` and `ChangeStateCommand` implement `IGameCommand`; a screen enqueues one into a
`CommandQueue`, and the queue is drained by `StateManager` at a point where no state's method is on
the call stack. The stack destruction therefore always happens between frames.

**Related files.** `src/core/StateManager.cpp` · `include/core/StateManager.h` · `include/command/StateCommands.h` · `include/command/IGameCommand.h` · `include/command/CommandQueue.h`

---

## F004 — Centralised asset ownership

**Subsystem:** Infrastructure

**What it does.** A texture used by fifty coins is loaded from disk exactly once and lives in video
memory exactly once. Nothing leaks when a level unloads, and switching rooms does not re-read the same
PNG files from the hard drive.

**How it works.** `AssetManager` is a singleton holding three `unordered_map` caches — textures, fonts
and sounds — keyed by a logical name rather than a file path. `loadTexture(name, path)` checks the map
first and only touches the disk on a miss; `getTexture(name)` returns a `const&` and logs a diagnostic
plus a zeroed fallback rather than crashing when a name is wrong. Entities and screens never own GPU
handles, so their destructors do no graphics work and can run in any order. The single `unloadAll()`
call at shutdown releases every texture, font and sound together, after which raylib's context is
closed. Bug history: level transitions once leaked one tileset texture per room change, which was
fixed by routing tileset loading through this cache instead of calling `LoadTexture` directly.

**Related files.** `src/infrastructure/AssetManager.cpp` · `include/infrastructure/AssetManager.h`

---

## F005 — Resolution-independent interface scaling

**Subsystem:** User interface

**What it does.** The menu, the panels and the buttons look identical on every machine. On a 1080p
laptop, a 1440p desktop and a 4K monitor at 200% display scaling, every element keeps the same
proportions and the same relative position; nothing is stretched, nothing drifts into a corner, and
nothing overlaps.

**How it works.** `UIScaler` defines a **virtual canvas** of 2560 × 1440 and every layout in the menu
is authored against that canvas in fixed pixel numbers. At runtime `Refresh()` reads the real
framebuffer and computes a single uniform factor, `min(w / 2560, h / 1440)`, plus a centring offset that
letterboxes the leftover space. Two families of accessor keep the distinction that actually matters:
`S(length)` converts a **size**, while `X(x)`, `Y(y)`, `Pos()` and `Rect()` convert a **position** and
therefore also add the centring offset. Because the factor is uniform, artwork is never distorted —
only made larger or smaller. The design resolution was chosen by measuring the real maximised
framebuffer on the development machine, so the factor is exactly 1.0 there and the layout is
pixel-identical to what was originally hand-tuned.

**Related files.** `include/ui/UIScaler.h` · `src/ui/UIScaler.cpp` · `src/states/MainMenuState.cpp`

---

## F006 — Four-channel audio system

**Subsystem:** Infrastructure and settings

**What it does.** The player controls four independent volumes — master, music, player sound effects
and enemy sound effects — from either the main-menu settings screen or the in-game settings panel.
The choices persist between sessions, so the game sounds the way it was left.

**How it works.** Sound ownership follows the same rule as every other asset: `AssetManager` holds an
`unordered_map<std::string, Sound>` and `loadSound(name, path)` decodes a clip once, verifies that
raylib actually produced a valid stream buffer before caching it, and logs a diagnostic instead of
caching a broken handle on failure. `getSound(name)` returns a shared `const&`, so a sound effect
triggered by fifty entities still occupies one buffer. The volume model lives in `SettingsManager`,
which stores `masterVolume_`, `bgmVolume_`, `playerSfxVolume_` and `enemySfxVolume_` as clamped
floats in `[0,1]`, writes to `session.json` on every change, and reloads them at startup. Separating
player and enemy channels means a player who finds combat noise fatiguing can quiet the enemies
without losing their own attack feedback. Clips are supplied by dropping files into the asset folder;
a missing clip is reported and skipped rather than aborting startup.

**Related files.** `src/infrastructure/AssetManager.cpp` · `include/infrastructure/AssetManager.h` · `src/core/SettingsManager.cpp` · `include/core/SettingsManager.h` · `src/ui/ButtonGroup.cpp` · `src/ui/IngameSettingsPanel.cpp`

---
---

# 2. Screens and navigation

## F007 — Playable main menu with demo AI

**Subsystem:** Screens

**What it does.** The main menu is not a picture. Behind the buttons a real level is running, with two
real characters fighting each other, real physics and a real camera. The game is already alive before
the player presses anything.

**How it works.** `MainMenuState` constructs a genuine `TileMap`, loads a campaign room into it, and
spawns two `Player` objects through the same `PlayerFactory` used in gameplay. Rather than reading the
keyboard, it drives those players from a small autonomous controller: randomised timers decide when
each demo character walks, turns, jumps or launches an attack, and the same skill and combat code paths
run as they would for a human. This is deliberate reuse rather than a special case — if movement or
combat breaks, the menu shows it immediately. The menu interface is then drawn over the top with the
`UIScaler` virtual canvas, and the demo continues running underneath while panels open and close.

**Related files.** `src/states/MainMenuState.cpp` · `include/states/MainMenuState.h` · `src/entity/Player/PlayerFactory.cpp`

---

## F008 — Animated menu shell, title entry and character codex

**Subsystem:** User interface

**What it does.** Menu buttons slide in one after another rather than appearing all at once, each with
its own hover and pressed artwork. The title eases into place with a slight overshoot. A codex panel
lets the player browse all six playable characters before starting, and panels open and close with a
page-turn animation instead of a hard cut.

**How it works.** `ButtonGroup` is the shared widget host: it owns a vector of button records, each
holding two textures (normal and pressed), a bounding rectangle, a hover scale, an `std::function<void()>`
click action and a per-button **entry delay**. Staggering is achieved by giving successive buttons
increasing delays, so one timer produces the cascade. The title uses an easing curve that overshoots
its target and settles back. `CharacterInfoPanel` implements `IMenuPanel` and owns two `UIAtlasAnimator`
instances — one for the opening page-turn, one for the closing one — so the panel's appearance and
disappearance are sprite sequences rather than alpha fades. Because every panel implements the same
`IMenuPanel` interface (`Update`, `Render`, `HandleInput`, `TriggerEntry`, `TriggerExit`,
`SetOnCloseCallback`), the menu screen drives all of them through one uniform loop.

**Related files.** `src/ui/ButtonGroup.cpp` · `src/ui/CharacterInfoPanel.cpp` · `src/ui/UIAtlasAnimator.cpp` · `include/ui/IMenuPanel.h` · `src/states/MainMenuState.cpp`

---

## F009 — World selection with new-game / load-game choice

**Subsystem:** Screens and save system

**What it does.** After choosing 1-player mode the player sees six world nodes on a map. Clicking one
does not immediately start a run — a panel opens over the map offering **NEW GAME** or **LOAD GAME**,
with a back button to change their mind.

**How it works.** `MapSelectionState` draws the six nodes with hover scaling and hit-tests them against
`UIScaler`-converted rectangles. Selecting a node calls `WorldActionPanel::Open(worldIndex, worldName)`
rather than a state change, so the panel is a modal layer over the same screen. The panel is another
`IMenuPanel` implementation and owns three `PanelButton` widgets; it exposes `SetOnNewGame` and
`SetOnLoadGame` callbacks, which the screen binds to two different outcomes — one enqueues a state
change to character selection, the other opens `SaveVersionPanel` in place. Crucially the screen never
hard-codes what "world 3" means: it looks the world up in `WorldCatalog`, a registry that maps a world
index to its display name, its `.ldtk` file, its save folder and a factory that constructs the right
level screen. Adding a world is a registry entry, not a new `switch` branch.

**Related files.** `src/states/MapSelectionState.cpp` · `src/ui/WorldActionPanel.cpp` · `include/ui/PanelButton.h` · `src/states/WorldCatalog.cpp` · `include/states/WorldDescriptor.h`

---

## F010 — Save browser

**Subsystem:** Screens and save system

**What it does.** Choosing LOAD GAME lists every save recorded for that world. Each row shows which
character was being played, which room the run reached, how many coins were collected, the character's
health and how long the run has lasted. Selecting a row reveals three actions — **DELETE**, **BACK**,
**LOAD** — so a save can be inspected before committing to it and removed if it is no longer wanted.

**How it works.** `SaveVersionPanel` is populated with a `std::vector<SaveSlotInfo>` supplied by the
repository layer. `SaveSlotInfo` is a deliberately small summary struct — it exists so the browser can
render a list of twenty saves without deserialising twenty complete world snapshots. The panel tracks
`selectedIndex_` and only shows the action row once a slot is selected, and it communicates outward
purely through callbacks (`SetOnLoad`, `SetOnDelete`) rather than reaching into the save system itself.
Deleting refreshes the list in place through `Refresh(...)` instead of tearing the panel down, so the
scroll position and the panel animation survive the operation.

**Related files.** `src/ui/SaveVersionPanel.cpp` · `include/save/SaveSlotInfo.h` · `src/save/FileSaveRepository.cpp` · `include/save/ISaveRepository.h`

---

## F011 — Character selection with live preview and skill demo

**Subsystem:** Screens

**What it does.** Six character cards are laid out over an animated background. Hovering a card lifts
that character onto a floating island in the centre of the screen, where they idle and periodically
perform one of their attack animations — so the player can see how a character actually moves before
committing to a whole run. In two-player modes the screen runs a second time for player 2, with a
transition between the two picks and the first player's choice remaining visible.

**How it works.** `CharacterSelectionState` builds a real `Player` for the hovered character through
`PlayerFactory`, which means the preview uses the character's genuine animation set and genuine skill
timings rather than a separate preview asset. A timer periodically drives that preview player into a
skill state and lets it return to idle. The background is a decoded animated GIF played through
`GifAnimation`, with a 50%-opacity black overlay composited over it so the character cards stay
readable against a moving image. Two-player selection is handled by a pass counter inside the same
screen rather than by a second screen class: after player 1 confirms, the screen resets its selection,
increments the pass, and plays an iris transition.

**Related files.** `src/states/CharacterSelectionState.cpp` · `src/entity/Player/PlayerFactory.cpp` · `src/infrastructure/GifAnimation.cpp` · `src/ui/transitions/IrisTransition.cpp`

---

## F012 — Loading screen with deferred construction

**Subsystem:** Screens

**What it does.** Moving between major screens shows an animated loading screen rather than a frozen
window. The game never appears to have crashed while a world is being built.

**How it works.** `LoadingState` is constructed with two things: a `std::function` **factory** that
will build the target screen, and a duration. It plays its animation for part of that duration, then
invokes the factory partway through — so the expensive work (parsing a `.ldtk` project, decoding
tilesets, constructing dozens of entities) happens while an animation is already on screen and the
player is already looking at motion. When construction finishes and the animation completes, the
loading screen enqueues a `ChangeStateCommand` carrying the newly built screen. Passing a factory
rather than a finished screen is what makes the deferral possible: the caller describes *what* to
build without paying for it at the call site.

**Related files.** `src/states/LoadingState.cpp` · `include/states/LoadingState.h` · `include/command/StateCommands.h`

---

## F013 — Endgame screen with two outcomes

**Subsystem:** Screens

**What it does.** Finishing a run shows a completion screen. In story and co-op modes it is a
"level completed" screen. In versus mode it is a winner's podium showing the victorious character
animating in the centre of the frame. Both offer a return-to-menu button in the lower-right corner.

**How it works.** `EndgameState` takes the outcome as construction data and selects one of two
background images accordingly. All positioning is expressed as a fraction of the current framebuffer
rather than in absolute pixels, so the return button sits in the same relative place regardless of
window size. For the versus variant the winning character's identity is passed through, and the screen
constructs that character's idle animation and draws it centred using the same framing arithmetic the
character-selection island uses — the winner is presented at the same scale and position the player
already associates with "this is your character". The return button enqueues a state change back to the
main menu through the deferred command queue rather than popping the stack directly.

**Related files.** `src/states/EndgameState.cpp` · `include/states/EndgameState.h` · `src/states/BaseLevelState.cpp`

---
---

# 3. Movement and terrain

## F014 — Ground movement

**Subsystem:** Player

**What it does.** The character runs left and right at a speed that differs per character, with the
sprite flipping to face the direction of travel, and settles into an idle animation when input stops.

**How it works.** Movement is not an `if` inside a large update function — it is a **state**.
`PlayerIdleState` and `PlayerRunState` both derive from `PlayerState`, and each owns its own
`handleInput`, `update` and `enter/exit` behaviour. Idle transitions to Run when a horizontal binding
is held; Run transitions back to Idle when it is released. The horizontal speed is read from the
character's `CharacterBaseStats`, populated from `characters.json`, so Luffy is genuinely slower than
Sasuke rather than sharing one constant. `Player` holds its states **by value** in a small struct of
concrete state objects, so a transition costs a pointer reassignment with zero heap allocation —
important because movement states change many times per second.

**Related files.** `src/entity/Player/PlayerIdleState.cpp` · `src/entity/Player/PlayerRunState.cpp` · `include/entity/Player/PlayerState.h` · `include/entity/Player/CharacterStats.h`

---

## F015 — Jump and fall

**Subsystem:** Player and physics

**What it does.** The character jumps with a per-character strength, arcs under gravity, and can act
differently while rising than while falling — including using a dedicated aerial attack.

**How it works.** Rising and falling are two separate states, `PlayerJumpState` and `PlayerFallState`,
rather than one airborne state with a sign test. Splitting them means each can carry its own animation,
its own permitted transitions and its own input rules without a nest of conditionals. Jump applies an
instantaneous negative vertical velocity taken from `CharacterBaseStats::jumpStrength`; gravity is then
integrated by the shared physics routine in `Entity`, which is where every moving object — players,
enemies, thrown bombs — gets its motion. The transition from Jump to Fall happens when vertical velocity
crosses zero, and the transition out of Fall happens when the physics pass reports a landing.

**Related files.** `src/entity/Player/PlayerJumpState.cpp` · `src/entity/Player/PlayerFallState.cpp` · `src/entity/Entity.cpp` · `include/entity/Entity.h`

---

## F016 — Crouch, reduced hitbox and one-way drop-through

**Subsystem:** Player and physics

**What it does.** Crouching makes the character physically shorter, so they fit through gaps that block
them standing up. Crouching while standing on a one-way platform and pressing jump drops them through
it instead of jumping.

**How it works.** `PlayerCrouchState` swaps the player's collision rectangle for a shorter one on
`enter()` and restores it on `exit()`. Because the collision box is data on the entity rather than a
constant inside the collision routine, no collision code needs to know that crouching exists. The
drop-through is implemented inside the shared collision resolution: a tile marked `OneWay` only blocks
an entity that is moving downward **and** whose feet were above the tile's top edge in the previous
step. When the player requests a drop-through, that check is suppressed for a short window, letting
them pass. The same mechanism is what allows any entity to jump up through a platform from below.

**Related files.** `src/entity/Player/PlayerCrouchState.cpp` · `src/entity/Entity.cpp` · `include/environment/TileMap.h`

---

## F017 — Ladder and vine climbing

**Subsystem:** Player and physics

**What it does.** Ladder and vine tiles can be climbed up and down, and the character can hang
motionless on them without falling.

**How it works.** Overlapping a `Ladder` or `Vine` tile makes the climb transition available;
`PlayerClimbState` then takes over and changes the physics contract for as long as it is active.
Gravity is disabled, vertical input maps directly to vertical velocity, and the climbing animation is
paused when there is no input so the character visibly grips the ladder rather than looping an
animation in place. Leaving the ladder — by jumping off, by reaching the top, or by walking off the
side — restores normal gravity through the state's `exit()`. Because gravity suppression lives in the
state rather than in a boolean on the player, it is impossible for a player to leave the ladder and
keep floating: the state object that suppressed gravity no longer exists.

**Related files.** `src/entity/Player/PlayerClimbState.cpp` · `src/entity/Entity.cpp` · `include/environment/TileMap.h`

---

## F018 — Swimming

**Subsystem:** Player and physics

**What it does.** Entering water changes how the character moves entirely: they swim in all four
directions with a slower, floatier feel, use a swimming animation set, and begin consuming breath.

**How it works.** Water contact pushes the player into `PlayerSwimState`, which replaces the normal
grounded physics model. Gravity is reduced to a gentle sink rather than removed, so the character
drifts downward when idle; vertical input adds upward velocity in strokes; horizontal speed is scaled
below the running speed. The state also starts the breath drain described in F031. Exiting water
returns the player to Fall rather than Idle, so surfacing produces the correct arc. Water is one of the
thirteen values in the collision grid, which means whether the player is swimming is derived from the
map data itself rather than from an entity flag someone has to remember to clear.

**Related files.** `src/entity/Player/PlayerSwimState.cpp` · `src/entity/Player/Player.cpp` · `src/entity/Entity.cpp`

---

## F019 — Thirteen tile collision behaviours

**Subsystem:** Environment and physics

**What it does.** The terrain is not simply "solid or empty". Thirteen distinct tile behaviours give
level designers real vocabulary: `None`, `Solid`, `OneWay`, `Hazard`, `Ladder`, `Water`, `Die`,
`Lotus`, `Cloud`, `Poison`, `Lava`, `Slop` and `Vine`. Clouds let a fast-moving character pass through
but hold up a slow one. Lily pads float as standing surfaces on water. Slopes are walked up and down
smoothly instead of being climbed like stairs. Lava and poison apply damage over time. `Die` kills on
contact.

**How it works.** `TileMap` stores a `vector<vector<CollisionType>>` parallel to the visual tile layers,
built when the level loads. Every moving entity calls `GetCollidingTiles(rect)`, which returns only the
tiles overlapping its bounding box — a grid lookup, not a scan of the map. `Entity::updatePhysicsWithMap`
then resolves the axes separately (horizontal first, then vertical, which is what makes slopes and
corners behave) and dispatches on the collision type. Because the behaviour table lives in one
function shared by every entity, an enemy standing in lava burns by exactly the same code that burns
the player. On top of that, the routine reports events back through virtual callbacks —
`onWallHit`, `onCeilingHit`, `onLanded`, `onLedgeAhead` — which the player uses for landing recovery
and which the enemy AI uses to turn around instead of walking off a cliff. A real bug found during this work: every `.ldtk` file writes the slope tag as `"Slope"`, but the
loader compared it against `"Slop"` only — so every slope in world 5 silently had no collision at
all. The loader now accepts both spellings.

**Related files.** `include/environment/TileMap.h` · `src/environment/TileMap.cpp` · `src/entity/Entity.cpp` · `include/entity/Entity.h`

---
---

# 4. Combat and skills

## F020 — Four-hit melee combo

**Subsystem:** Combat

**What it does.** Pressing the attack key repeatedly with correct timing chains four different attacks
together — Attack 1 → 2 → 3 → 4 — each with its own animation, its own damage, its own reach and its
own rhythm. Mistiming the follow-up drops the chain back to the first attack.

**How it works.** Each attack is a separate class deriving from `ISkill`: `Attack1Skill` through
`Attack4Skill`. `Player` owns a `map<string, unique_ptr<ISkill>>` built at construction from the
character's JSON entry, so "which four attacks does Goku have" is data. Pressing attack enters
`PlayerSkillState`, which runs the named skill and, when the skill's active window closes, checks
whether another attack press arrived inside the follow-up window. If it did, the state moves directly
to the successor skill rather than returning to idle. Each skill's damage, hitbox rectangle, reach and
timing come from `setCombatData`, `setDurationAndHitbox` and `setPacingData`, all fed from
configuration, so tuning combat means editing JSON.

**Related files.** `src/entity/Skill/Attack1Skill.cpp` … `Attack4Skill.cpp` · `include/entity/Skill/ISkill.h` · `src/entity/Player/PlayerSkillState.cpp`

---

## F021 — Data-driven combo chaining

**Subsystem:** Combat

**What it does.** The combo order is not written into the code. Each attack knows only the name of the
attack that may follow it, which means a character can have a different chain, a shorter chain, or a
chain that loops — purely by configuration.

**How it works.** `ISkill` carries a `nextComboSkillName` string and a `hasNextCombo()` query. When a
skill finishes, `PlayerSkillState` asks the skill what may come next and looks that name up in the
player's skill map. Neither the player nor the state machine contains the sequence 1→2→3→4 anywhere;
each skill names only its own successor, and the chain is the emergent result. This is a small design
decision with a large consequence: adding a fifth attack to one character is a JSON edit plus one skill
class, with no change to the state machine, the player, or the other three attacks. It also removes the
class of bug where a chain is updated in one place and not another, because there is only one place.

**Related files.** `include/entity/Skill/ISkill.h` · `src/entity/Player/PlayerSkillState.cpp` · `assets/config/characters.json`

---

## F022 — Aerial and crouching attacks

**Subsystem:** Combat

**What it does.** Attacking in mid-air performs a dedicated jump attack with its own arc and hitbox;
attacking from a crouch performs a low sweep that hits targets a standing attack would miss. Both are
distinct moves, not the ground combo replayed in a different pose.

**How it works.** `JumpAttackSkill` and `LowAttackSkill` are ordinary `ISkill` implementations, and the
decision of which to run is made by the state the player is *in* when the attack key is pressed. The
airborne states route the attack to the jump attack; `PlayerCrouchState` routes it to the low attack;
the grounded states route it into the combo chain. Because the routing is a property of the state
rather than a conditional inside a shared attack handler, adding a new stance with its own attack does
not require editing any existing attack code. Each carries its own hit window and its own recovery, so
a jump attack that lands is punished or rewarded differently from a ground attack.

**Related files.** `src/entity/Skill/JumpAttackSkill.cpp` · `src/entity/Skill/LowAttackSkill.cpp` · `src/entity/Player/PlayerCrouchState.cpp` · `src/entity/Player/PlayerJumpState.cpp`

---

## F023 — Projectile and special attacks

**Subsystem:** Combat

**What it does.** Every character has a ranged attack and a stronger special attack bound to its own
key. The projectile has that character's own artwork, speed, damage and lifetime — Goku's energy blast
and Zoro's slash are different objects with different numbers, not one recoloured sprite.

**How it works.** `LongAttackSkill` and `SpecialSkillAttack` spawn a `Fireball` configured by a
`FireballConfig` struct that the player loaded from `characters.json`: texture name, frame count,
speed, damage, lifetime, hitbox size, and whether the trajectory is straight or a sine curve. The
projectile itself is a full `Entity`, so it collides with terrain and is resolved by the same physics
routine as everything else. Spawning raises the interesting ownership question — a skill executing
inside a player must not reach into the level to insert an entity mid-iteration — and the project
answers it with a **spawn request queue**: the skill pushes a `SpawnCommand` describing what to create,
and the level drains that queue and constructs the projectile at a safe point during `Process`, when
nothing is iterating the entity list.

**Related files.** `src/entity/Skill/LongAttackSkill.cpp` · `src/entity/Skill/SpecialSkillAttack.cpp` · `src/entity/Fireball.cpp` · `include/entity/FireballConfig.h`

---

## F024 — Area explosion attack

**Subsystem:** Combat

**What it does.** The heaviest attack in a combo, and several enemy attacks, produce an explosion that
damages everything inside a radius rather than a single target in front of the attacker.

**How it works.** `Explosion` is an entity configured by `ExplosionConfig` — radius, damage, animation
name, frame count and duration. It exists for the length of its animation, contributes one hitbox to
the combat system for the frames its damage window is open, and then removes itself. Because it is an
entity rather than an effect drawn by the attacker, it keeps damaging correctly even if the attacker
dies, moves away, or is interrupted during the explosion. The same class serves the player's fourth
combo hit and the enemy explosion archetype (F044), which means one implementation, one set of
timings and one place to fix a bug.

**Related files.** `src/entity/Explosion.cpp` · `include/entity/ExplosionConfig.h` · `src/entity/Skill/Attack4Skill.cpp` · `src/entity/Skill/ExplosionEnemySkill.cpp`

---

## F025 — Dash and block

**Subsystem:** Combat

**What it does.** Dash gives a burst of movement for repositioning or closing distance. Block puts the
character into a guarding stance that reduces incoming damage instead of avoiding it.

**How it works.** Both are `ISkill` implementations, which is what makes them interesting: a movement
ability and a defensive ability go through exactly the same interface as an attack. `DashSkill` carries
a `dashMultiplier` applied to horizontal velocity for its duration. `BlockSkill` sets a `defensePower`
value that the combat system subtracts from incoming damage during resolution, so blocking is expressed
in the same damage arithmetic as everything else rather than as a special case in the hurt path. Both
consume mana like any other skill, and both occupy the skill state, so they cannot overlap an attack.
Modelling them as skills rather than as flags on the player is why they compose correctly with combo
chaining, hit windows and the mana economy without any of those systems knowing they exist.

**Related files.** `src/entity/Skill/DashSkill.cpp` · `src/entity/Skill/BlockSkill.cpp` · `include/entity/Skill/ISkill.h` · `src/combatsystem/CombatSystem.cpp`

---

## F026 — Frame-accurate hit windows and attack pacing

**Subsystem:** Combat

**What it does.** An attack only damages during the part of its animation where the weapon is actually
extended. Every move has a distinct feel: a wind-up before it can hit, an active window, a recovery
period during which the character is committed, and a brief freeze on impact that makes a hit feel
heavy.

**How it works.** `ISkill` stores `hitboxStartTime` and `hitboxEndTime` alongside
`anticipationDuration`, `recoveryDuration` and `hitStopDuration`. `PlayerSkillState` advances a timer
through the skill and only submits the skill's hitbox rectangle to `CombatSystem` while the timer lies
inside the active window; outside it, the attack is visually happening but deals nothing. Because the
window is expressed in seconds and the simulation runs at a fixed timestep (F001), the window is
identical on every machine — this is the practical reason the fixed timestep matters, not an abstract
one. Hit-stop is applied on a successful hit by briefly suspending the attacker's animation advance,
which is what separates a connecting hit from a whiff without any additional art.

**Related files.** `include/entity/Skill/ISkill.h` · `src/entity/Player/PlayerSkillState.cpp` · `src/combatsystem/CombatSystem.cpp`

---

## F027 — Faction-based damage filtering

**Subsystem:** Combat

**What it does.** Enemies never damage each other. Co-op partners never damage each other. In versus
mode, and only in versus mode, players *do* damage each other. A bomb thrown by a player hurts enemies
but not the thrower.

**How it works.** Every entity carries an `EntityFaction`, and every `Hitbox` carries a
`targetFactionMask` — a bitmask of the factions it is allowed to hit — plus an optional
`ignoreEntity` pointer for the specific case of a projectile passing through its own owner. Resolution
is a single `canHit(EntityFaction)` test rather than a chain of type checks. This is why enabling PvP
required no new combat code: the versus level simply builds the two players' hitboxes with a mask that
includes the other player's faction. Replacing type-based questions ("is this a Player and is that a
Mob?") with a data-based one ("does this mask include that faction?") is what keeps the combat system
independent of the entity hierarchy.

**Related files.** `include/entity/EntityFaction.h` · `include/combatsystem/Hitbox.h` · `src/combatsystem/CombatSystem.cpp`

---

## F028 — Central combat resolution and knockback

**Subsystem:** Combat

**What it does.** All damage in the game is decided in one place, once per frame. A hit pushes its
target away from the attacker, plays a recoil, applies invincibility frames, and spawns a damage number.

**How it works.** Entities do not damage each other directly. Each frame, `CombatSystem::update`
collects every active hitbox and every damageable entity, hands both lists to an `ICollisionDetector`,
and receives back a list of `CollisionPair`s. Damage, defence subtraction, knockback direction and
hit-stop are then applied uniformly. The detector is an injected interface: `BruteForceDetector` tests
every hitbox against every entity, which is entirely adequate at this project's entity counts, and can
be replaced by a spatial-grid implementation without touching a line of the resolution logic. Knockback
direction is computed from the attacker-to-target vector, so a hit from the left always pushes right
regardless of which attack caused it. Centralising resolution is also what makes the debug hitbox
renderer possible — one place knows about every hitbox in the frame.

**Related files.** `src/combatsystem/CombatSystem.cpp` · `src/combatsystem/BruteForceDetector.cpp` · `include/combatsystem/ICollisionDetector.h` · `include/combatsystem/Hitbox.h`

---
---

# 5. Survival statistics and status

## F029 — Health, death and respawn

**Subsystem:** Player

**What it does.** Each character has a health pool sized to that character. Reaching zero plays a death
animation, and only when that animation has finished does the player respawn at their last checkpoint —
with the world returned to the state it was in when that checkpoint was taken.

**How it works.** Health lives in `Player` and its maximum comes from `CharacterBaseStats`, so Luffy's 350
and Kakashi's 260 are configuration rather than constants. Death is a state, `PlayerDieState`, not a
boolean: entering it disables input handling, plays the death animation, and signals completion only on
the final frame. That sequencing matters in versus mode, where the winner screen must not appear until
the loser's death animation has actually played out. `BaseLevelState` observes the completed death and
performs the respawn, which restores both the player's position and the recorded world state (F066).

**Related files.** `src/entity/Player/PlayerDieState.cpp` · `src/entity/Player/Player.cpp` · `src/states/BaseLevelState.cpp` · `include/entity/Player/CharacterStats.h`

---

## F030 — Mana economy

**Subsystem:** Player

**What it does.** Skills cost mana. Mana regenerates steadily, so heavy skills cannot be spammed but
are always eventually available. Attempting a skill without enough mana produces a visible message
rather than silently doing nothing.

**How it works.** Each `ISkill` declares a `manaCost`. `Player::useSkill(name)` checks the pool before
entering the skill state and refuses the transition if it is short, emitting a `FloatingText` so the
failure is legible. Regeneration is a per-second rate applied in `Update`, which means it is tied to
the fixed timestep and therefore identical across machines. Because the cost lives on the skill rather
than in a lookup table, adding a skill cannot desynchronise its cost from its behaviour, and a
character's whole cost curve can be retuned from `characters.json` without recompiling.

**Related files.** `src/entity/Player/Player.cpp` · `include/entity/Skill/ISkill.h` · `include/entity/FloatingText.h`

---

## F031 — Underwater breath and drowning

**Subsystem:** Player

**What it does.** Staying underwater drains a breath meter shown on the interface. When it empties the
character starts drowning and takes damage at intervals. Surfacing refills it.

**How it works.** The breath value is a counter on `Player`, decremented while the player is in
`PlayerSwimState` and refilled when they are not. At zero, a periodic timer applies damage directly
rather than through the combat system, because drowning has no attacker and no hitbox — routing it
through `CombatSystem` would have meant inventing a fake hitbox for the water. This is an example of
the project deliberately *not* forcing a mechanism where it does not fit. The meter is drawn by
`PlayerHUD` alongside health and mana, and the value is captured in saves (F063) so reloading does not
hand the player a full lungful of air they had not earned.

**Related files.** `src/entity/Player/Player.cpp` · `src/entity/Player/PlayerSwimState.cpp` · `src/ui/PlayerHUD.cpp` · `include/save/PlayerSaveData.h`

---

## F032 — Invincibility frames and hurt interruption

**Subsystem:** Player and combat

**What it does.** Taking a hit interrupts whatever the character was doing, plays a recoil, and grants
a brief window of immunity during which the character visibly flashes. Without it, standing in a group
of enemies would drain a full health bar in a fraction of a second.

**How it works.** A successful hit drives the player into `PlayerHurtState`, which cancels the current
skill, applies knockback and starts an invincibility timer. While that timer runs, `Player` rejects
incoming damage before it reaches health arithmetic, and its render path alternates visibility so the
state is readable rather than invisible. Interruption is handled by the state machine's own transition
rules rather than by flags: because the hurt state *replaces* the skill state, there is no path by
which a cancelled attack can continue to emit a hitbox. This is the concrete payoff of modelling
behaviour as states — cancellation is structurally impossible to get wrong.

**Related files.** `src/entity/Player/PlayerHurtState.cpp` · `src/entity/Player/Player.cpp` · `src/combatsystem/CombatSystem.cpp`

---

## F033 — Poison and burning damage over time

**Subsystem:** Effects

**What it does.** Standing in poison applies a lingering effect that damages periodically and slows
movement, drawn as a green overlay on the character. Lava applies a burning effect with its own overlay
and its own damage rate. Both continue after the character has left the hazard.

**How it works.** `IEffect` is the status-effect interface, with `PoisonEffect` and `LavaEffect`
implementing it. Each carries a duration, a tick interval, a per-tick damage value, an optional
movement modifier and an animated overlay, and is attached to the entity that acquired it. The tile
types `Poison` and `Lava` from the collision grid apply them on contact, and the thrown poison flask
(F051) applies the same poison effect — one implementation serving both a terrain hazard and a
throwable weapon. Because the effect is attached to the entity rather than to the tile, it survives the
character walking away, which is the entire point of a damage-over-time mechanic. Overlays are drawn in
the entity's render pass, so they follow the character correctly through every animation.

**Related files.** `src/entity/Effects.cpp` · `include/entity/Effects.h` · `src/entity/Entity.cpp` · `src/entity/Item/PoisonFlask.cpp`

---

## F034 — Floating combat text

**Subsystem:** Presentation

**What it does.** Damage numbers rise from a character when they are hit, healing ticks appear in a
different colour, and status messages such as an insufficient-mana warning float above the player.
The player can read what is happening without decoding health-bar movement.

**How it works.** `FloatingText` is a lightweight object holding a string, a colour, a world position,
a lifetime and a rise speed. Instances are owned by the level, updated once per fixed step and drawn in
world space so they scroll with the camera and stay anchored to where the event happened. They are
deliberately not entities: they have no collision, no faction and no physics, and giving them the full
`Entity` interface would have meant carrying five unused subsystems for a rising number. Choosing the
smaller abstraction here is a considered decision rather than an omission.

**Related files.** `include/entity/FloatingText.h` · `src/entity/Player/Player.cpp` · `src/combatsystem/CombatSystem.cpp`

---
---

# 6. Playable characters

## F035 — Six playable characters

**Subsystem:** Characters

**What it does.** Goku, Naruto, Luffy, Kakashi, Sasuke and Zoro are each fully playable across every
mode — story, co-op and versus — with complete sprite sets, distinct movesets and distinct feel.
Character choice is a real tactical decision rather than a skin.

**How it works.** No character has a class of its own. There is one `Player` class, and six entries in
`assets/config/characters.json`. Each entry supplies statistics, an animation table, a skill list, and
the configuration for that character's projectiles and explosions. `PlayerFactory::create(name)` reads
the entry and assembles a fully configured `Player`. This is why the character-selection preview can
show real behaviour (F011) and why the menu demo can fight with real characters (F007) — there is only
one implementation to keep consistent. It is also why adding a seventh character requires no new class.

**Related files.** `assets/config/characters.json` · `src/entity/Player/PlayerFactory.cpp` · `src/entity/Player/Player.cpp`

---

## F036 — Per-character statistics and animation sets

**Subsystem:** Characters

**What it does.** Characters genuinely play differently. Health ranges from 260 to 350, mana from 80 to
130, movement speed from 280 to 330, and jump strength varies as well — so Luffy is a durable, slow
bruiser and Kakashi is fragile and fast. Each character has seventeen animations (Sasuke has
twenty-one), covering idle, run, jump, fall, crouch, hurt, die, dash, the four combo attacks, the jump
and low attacks, block, the long attack and the special attack.

**How it works.** `CharacterBaseStats` is a plain data struct filled from JSON, and `Player` reads its
speed, jump strength, health and collision size from it rather than from constants. Animations are
declared in the same JSON entry as a table of name → sprite sheet, frame count, frame duration and
loop flag, from which `Animation` objects are built at construction. That an animation set can be a
different *size* per character — twenty-one for Sasuke — falls out naturally, because nothing in the
code assumes a fixed animation count.

**Related files.** `include/entity/Player/CharacterStats.h` · `assets/config/characters.json` · `src/entity/Animation.cpp` · `src/entity/Player/PlayerFactory.cpp`

---

## F037 — Per-character projectiles and explosion effects

**Subsystem:** Characters and combat

**What it does.** Each character's ranged attack looks and behaves like that character's ranged attack.
The artwork, frame count, travel speed, damage, lifetime, hitbox size, curvature and explosion radius
are all specific to the character firing it.

**How it works.** `FireballConfig` and `ExplosionConfig` are per-character data blocks loaded alongside
the rest of the character entry. When `LongAttackSkill` or `SpecialSkillAttack` fires, it does not
construct a hard-coded projectile — it passes the owning character's config to the `Fireball`
constructor. Curvature is a flag in the config that switches the projectile between straight-line
travel and a sine-modulated path, so a character can have a wavering energy blast without a second
projectile class. The same pattern applies to explosions. Two configuration structs therefore replace
what would otherwise have been twelve projectile subclasses.

**Related files.** `include/entity/FireballConfig.h` · `include/entity/ExplosionConfig.h` · `src/entity/Fireball.cpp` · `src/entity/Explosion.cpp`

---

## F038 — Data-driven character construction

**Subsystem:** Characters (architecture)

**What it does.** Adding a new playable character is a configuration edit and an art drop. No new class,
no new `switch` case, no change to the state machine, the combat system or the interface.

**How it works.** `PlayerFactory` is the single point where a name becomes a character. It reads the
JSON entry, fills `CharacterBaseStats`, builds the `Animation` table, constructs the `ISkill` objects named
in the entry and inserts them into the player's skill map, and attaches the projectile and explosion
configs. Everything downstream — the state machine, the combat system, the HUD, the save system, the
character-selection preview — consumes a `Player` and neither knows nor cares which of the six it is.
This is Simple Factory used for its real purpose: not to hide a `new`, but to keep the knowledge of
"what a character is made of" in exactly one place so it cannot drift out of sync.

**Related files.** `src/entity/Player/PlayerFactory.cpp` · `include/entity/Player/PlayerFactory.h` · `assets/config/characters.json`

---
---

# 7. Enemies, bosses and AI

## F039 — Nine enemy types

**Subsystem:** Enemies

**What it does.** Rat, bat, goblin, slime, mushroom, skeleton, soldier, tree and guardian populate the
worlds, and they are meaningfully different opponents. Health runs from 30 to 200, movement speed from
50 to 150, detection range from 100 to 400 pixels and attack range from 35 to 150 — so a rat is a fast
nuisance, a soldier snipes from a distance, and a guardian is a slow wall of health.

**How it works.** As with characters, there is no `Goblin` class. `Mob` is the single enemy class and
`assets/config/enemies.json` holds nine entries, each specifying statistics, an animation table, patrol
timing, detection and attack ranges, and the names of the skills the enemy may use.
`EnemyFactory::create(name, position)` reads the entry and returns a configured `Mob`. Every downstream
system — the AI state machine, the combat system, the save system that records which enemies are dead —
works against `Mob` and is unaffected by how many types exist.

**Related files.** `assets/config/enemies.json` · `src/entity/EnemyFactory.cpp` · `src/entity/Mob.cpp` · `include/entity/Mob.h`

---

## F040 — Six boss encounters

**Subsystem:** Enemies

**What it does.** Doflam, Franky, Itachi, Sasuke, Shank and Naruto are 1000-health set-piece fights with
their own move sets, their own detection and attack ranges, and a scripted entrance. They anchor the
end of a world rather than appearing as tougher regular enemies.

**How it works.** `Boss` extends the enemy model rather than duplicating it: it inherits the physics,
combat and animation machinery and adds a larger state machine (F042) plus the intro gating (F043).
Boss statistics and skills come from configuration in the same way regular enemies' do, so a boss is
distinguished by its *state machine* and its *data*, not by bespoke code. The inheritance edge here is
deliberate and narrow — `Boss` adds states and an intro condition, and nothing else — which is why the
combat system, the camera and the save system need no boss-specific branches.

**Related files.** `src/entity/Boss.cpp` · `include/entity/Boss.h` · `src/entity/Mob.cpp` · `assets/config/enemies.json`

---

## F041 — Seven-state enemy AI

**Subsystem:** Enemies and AI

**What it does.** Enemies behave rather than merely move. They idle, patrol a stretch of ground, notice
a player entering their detection radius, chase, attack when in range, recoil when hurt, and die. The
tempo of all of it — how long they idle, how far they patrol, how close they must be to notice or
strike — differs per enemy type.

**How it works.** `Mob` owns a `unique_ptr<IMobState>` and delegates its per-frame behaviour to it.
Seven concrete states implement `IMobState`: `EnemyIdleState`, `EnemyPatrolState`, `EnemyRunState`
(chase), `EnemyAttackState`, `EnemyHurtState`, `EnemyDieState` and `EnemySkillState`. Each state
decides its own exit conditions by querying the mob for distance to target, whether a cooldown has
elapsed, or whether a ledge lies ahead. Enemies hold their state through a `unique_ptr` rather than by
value as the player does, because enemy transitions are far less frequent and the pointer indirection
buys the freedom to add states without touching `Mob` — the two halves of the project use the same
pattern with two different trade-offs, chosen for two different access patterns.

**Related files.** `include/entity/IMobState.h` · `src/entity/EnemyStates/` (7 files) · `src/entity/Mob.cpp`

---

## F042 — Nine-state boss AI

**Subsystem:** Enemies and AI

**What it does.** Bosses have everything a regular enemy has plus a scripted entrance that plays before
the fight begins, and a development state used to drive the boss manually while tuning its behaviour.

**How it works.** `BossIdleState`, `BossPatrolState`, `BossRunState`, `BossAttackState`,
`BossSkillState`, `BossHurtState` and `BossDieState` mirror the enemy set, and two states are unique to
bosses: `BossIntroState`, which holds the boss in its entrance pose and refuses all combat transitions
until released, and `BossDebugInputState`, which routes keyboard input into the boss so a developer can
walk it through each attack and verify hitbox timing without fighting it legitimately. The debug state
is worth naming as a feature of the engineering rather than of the game: because behaviour is a state,
adding a manual-control mode required no change to `Boss` at all — just one more class implementing the
same interface.

**Related files.** `src/entity/BossStates/` (9 files) · `include/entity/BossStates/BossIntroState.h` · `src/entity/Boss.cpp`

---

## F043 — Cutscene-gated boss encounters

**Subsystem:** Enemies and cutscenes

**What it does.** A boss stands motionless in its entrance pose when the player first enters the arena.
The cutscene plays — camera moves, dialogue runs — and only when it ends does the boss engage. The fight
never starts underneath the dialogue.

**How it works.** `BossIntroState` is the gate. It holds the intro animation and refuses every transition
that would begin combat until `CutsceneManager` reports the associated cutscene complete. Because the
gate is a *state* rather than a boolean checked in several places, there is no code path by which the
boss can attack early — the states that could attack are not reachable yet. Releasing the gate hands
control to the ordinary idle state and the fight proceeds through the normal machine. This is the same
structural argument as hurt-interruption in F032, applied to a different problem.

**Related files.** `src/entity/BossStates/BossIntroState.cpp` · `src/entity/Boss.cpp` · `src/cutscene/CutsceneManager.cpp`

---

## F044 — Three enemy attack archetypes

**Subsystem:** Enemies and combat

**What it does.** Enemies attack in one of three ways — a melee swing, a fired projectile, or an
explosion — and which one they use is determined by their configuration, not their type. A soldier with
a 150-pixel attack range fires; a rat with a 35-pixel range swings.

**How it works.** `IEnemySkill` has three implementations: `BasicMeleeEnemySkill`, `ProjectileEnemySkill`
and `ExplosionEnemySkill`. Each enemy's JSON entry names the skills it may use, and `EnemyFactory`
constructs the matching objects and hands them to the mob. `EnemyAttackState` then simply asks the mob
for a skill and runs it, learning nothing about which of the three it received. Three implementations
therefore cover fifteen enemy and boss types, and the projectile archetype reuses `Fireball` while the
explosion archetype reuses `Explosion` — the same classes the player's attacks use, so a fix to
projectile collision fixes it for both sides at once.

**Related files.** `include/entity/Skill/IEnemySkill.h` · `src/entity/Skill/BasicMeleeEnemySkill.cpp` · `src/entity/Skill/ProjectileEnemySkill.cpp` · `src/entity/Skill/ExplosionEnemySkill.cpp`

---

## F045 — Enemy perception, ledge avoidance and target selection

**Subsystem:** Enemies and AI

**What it does.** Enemies do not walk off cliffs or wade into lava. They pick the nearest player who is
actually alive, ignore a player made invisible by a buff, and pace their attacks with a cooldown so they
pressure the player instead of machine-gunning them. In co-op an enemy will switch targets when the
other player comes closer.

**How it works.** Ledge and hazard avoidance ride on the physics callbacks described in F019: the shared
collision routine probes the tile ahead and below and reports `onLedgeAhead` when the ground stops or
turns into `Water`, `Lava` or `Poison`. Enemies configured as cautious respond by reversing their patrol
direction; enemies that should charge regardless simply ignore the callback. Target selection runs each
frame in `Mob`, filtering the player list by alive-and-visible and choosing the minimum distance, so
invisibility (F057) works against every enemy without the buff knowing enemies exist. Attack pacing is a
per-mob cooldown timer consulted by `EnemyAttackState` before it will re-enter.

**Related files.** `src/entity/Mob.cpp` · `src/entity/Entity.cpp` · `src/entity/EnemyStates/EnemyPatrolState.cpp` · `src/entity/EnemyStates/EnemyAttackState.cpp`

---
---

# 8. Items and interactables

## F046 — Coins, keys and the shared party inventory

**Subsystem:** Items

**What it does.** Coins are the currency; keys open doors. In two-player modes both are **pooled** —
either player can pick up a coin and both see the same total, and a key found by one player opens a door
for the other. Totals survive room changes and are written into saves.

**How it works.** `Coin` and `Key` are `BaseItem` subclasses that detect player overlap and, instead of
mutating a player, report into a shared `PartyInventory` held by the level. Pooling is therefore a
property of *where the counter lives*, not of special co-op code — single-player uses the same shared
inventory with one contributor. `PlayerHUD` reads that one object, which is why the two-player HUD shows
per-player health bars but a single coin and key total. Persistence is the same story: the inventory
serialises through `InventorySaveData`, a small DTO with no behaviour, so the save system never touches
item classes.

**Related files.** `src/entity/Item/Coin.cpp` · `src/entity/Item/Key.cpp` · `include/save/InventorySaveData.h` · `src/ui/PlayerHUD.cpp`

---

## F047 — Locked doors

**Subsystem:** Items

**What it does.** A door blocks a route until the party holds a key. Approaching with a key consumes it
and opens the door permanently; approaching without one leaves the door shut.

**How it works.** `Door` is a `BaseItem` that queries the shared `PartyInventory` on player contact.
Opening decrements the key count and flips the door's own state to open, and that state change is
recorded in `ItemState` so the door is still open when the player leaves the room and comes back — and
still open after a save is reloaded. The door does not remove itself from the level, because the level's
geometry depends on it; it stays as a non-blocking entity with an open sprite. This distinction between
"consumed" and "changed state" is exactly what the world-state persistence layer (F067) is built to
express.

**Related files.** `src/entity/Item/Door.cpp` · `include/entity/Item/ItemState.h` · `src/states/BaseLevelState.cpp`

---

## F048 — Lucky blocks and treasure chests

**Subsystem:** Items

**What it does.** Striking a block from below bounces it and dispenses an item. Chests can be opened for
loot, appear in several visual variants, and a rarer boss chest with its own artwork yields a key. Once
used, none of them can be farmed — an emptied block or an opened chest stays that way.

**How it works.** `LuckyBlock`, `ChestNormal` and `ChestBoss` all derive from `BaseItem` and share the
same lifecycle: detect the triggering contact, play a one-shot animation, and spawn their contents into
the world as real entities via the spawn queue rather than granting them invisibly. Spawning real items
means the reward physically drops, bounces and must be collected, which is both better feedback and less
special-case code. The used/unused flag is stored in `ItemState`, keyed by the entity's stable LDtk `iid`,
so it is preserved across room transitions and reloads. `ChestNormal` picks a visual variant at
construction from its available artwork, which keeps a corridor of chests from looking cloned.

**Related files.** `src/entity/Item/LuckyBlock.cpp` · `src/entity/Item/ChestNormal.cpp` · `src/entity/Item/ChestBoss.cpp` · `include/entity/Item/ItemState.h`

---

## F049 — Directional springs

**Subsystem:** Items

**What it does.** Springs launch the player, and they come in four orientations — up, down, left and
right — so a level designer can build vertical shafts, forced side-launches and drop chutes rather than
only trampolines.

**How it works.** `Spring` reads its orientation from the LDtk entity field placed by the level author
and applies an impulse along the corresponding axis on contact, then plays its compression animation.
The impulse is written into the entity's velocity and the rest is handled by the ordinary physics
integration — the spring does not teleport or animate the player itself, which is why a launched player
still collides correctly with everything on the way up. Reading orientation from level data rather than
having four spring classes means the editor's entity palette exposes one spring with a rotation, not
four separate placeables.

**Related files.** `src/entity/Item/Spring.cpp` · `include/entity/Item/Spring.h` · `src/entity/Entity.cpp`

---

## F050 — Checkpoint flags

**Subsystem:** Items and save system

**What it does.** Touching a flag does two things at once: it fixes the point the player will respawn at
after dying, and it writes a permanent save of the whole run to disk. Flags are therefore both the
in-session safety net and the only thing that creates a save file.

**How it works.** `Flag` reports the touch to `BaseLevelState`, which captures the complete world
snapshot and hands it to the save repository. Making the flag the sole save trigger is a deliberate
design choice: the player always knows when they have saved, and the save list becomes a readable
history of progress rather than a pile of autosaves. A flag that has already been touched marks itself
so it does not write a second identical version on the way back through.

**Related files.** `src/entity/Item/Flag.cpp` · `src/states/BaseLevelState.cpp` · `src/save/FileSaveRepository.cpp`

---

## F051 — Throwable bombs and poison flasks

**Subsystem:** Items

**What it does.** Bombs and poison flasks are carried, then thrown. A bomb arcs, lands, and explodes
after a fuse, damaging everything in radius. A flask shatters on impact and leaves a poisoned patch
that damages anything standing in it.

**How it works.** Both are `BaseItem` subclasses when lying on the ground, and both become physics
entities when thrown — the throw sets an initial velocity and the ordinary integration in `Entity`
produces the arc, so a bomb bounces off walls and rolls down slopes without any bespoke trajectory code.
On detonation the bomb constructs an `Explosion` (F024); the flask applies the same poison effect the
`Poison` terrain type applies (F033). Throwing is not implemented in the item at all: it lives in
`ThrowBombStrategy` and `ThrowPoisonStrategy`, two interchangeable use behaviours selected at the moment
of use (F053).

**Related files.** `src/entity/Item/Boom.cpp` · `src/entity/Item/PoisonFlask.cpp` · `include/entity/Item/ThrowBombStrategy.h` · `include/entity/Item/ThrowPoisonStrategy.h`

---

## F052 — One-slot inventory with swapping

**Subsystem:** Items and player

**What it does.** The player carries exactly one item at a time, shown on the HUD. Standing over a
different item and picking it up **swaps** the two — the held item is dropped where the new one was — so
the player is always making a choice rather than hoarding.

**How it works.** `Player` holds a single stored-item slot rather than a container. Pickup checks whether
the slot is occupied; if it is, the current item is re-spawned into the world at the pickup position
before the new one is stored. The one-slot limit is a game-design decision with a pleasant engineering
consequence: there is no inventory UI, no stack merging and no capacity logic anywhere in the codebase.
The slot is captured in `PlayerSaveData`, so a run reloaded from a save still has the bomb the player was
carrying.

**Related files.** `src/entity/Player/Player.cpp` · `include/save/PlayerSaveData.h` · `src/ui/PlayerHUD.cpp`

---

## F053 — Context-sensitive item use

**Subsystem:** Items (architecture)

**What it does.** One key uses whatever is held. Holding a buff potion consumes it; holding a bomb throws
it; holding a flask throws that instead. The player learns one control that does the right thing.

**How it works.** `Player::useStoredItem()` does not test what it is holding. It asks
`ItemUsageFactory::create(itemIdentifier)` for an `IItemUseStrategy` and calls the single method
`use(Player&)` on whatever comes back. `ConsumeBuffStrategy` applies a named buff; `ThrowBombStrategy`
and `ThrowPoisonStrategy` launch their projectile. This is the cleanest Strategy application in the
project — a one-method interface, chosen by a factory, invoked by a context that learns nothing about
which implementation ran — and it means adding a new usable item type requires one strategy class and one
factory line, with `Player` untouched.

**Related files.** `include/entity/Item/IItemUseStrategy.h` · `src/entity/Item/ItemUsageFactory.cpp` · `include/entity/Item/ConsumeBuffStrategy.h` · `src/entity/Player/Player.cpp`

---

## F054 — Level exit gate

**Subsystem:** Items and progression

**What it does.** Each world ends at an animated gate. Reaching it finishes the run and opens the
completion screen.

**How it works.** `EndgameAsset` is a `BaseItem` whose visual is a decoded animated GIF played through
`GifAnimation`, so the gate genuinely animates rather than looping a sprite strip. On player contact it
signals `BaseLevelState`, which stops accepting input, plays the outgoing iris transition and enqueues a
change to `EndgameState` with the outcome. Routing completion through the same deferred command queue as
every other transition means the gate cannot destroy the level it is standing in.

**Related files.** `src/entity/Item/EndgameAsset.cpp` · `src/infrastructure/GifAnimation.cpp` · `src/states/BaseLevelState.cpp` · `src/states/EndgameState.cpp`

---
---

# 9. Buffs and power-ups

## F055 — Movement and strength buffs

**Subsystem:** Buffs

**What it does.** Speed, jump and strength buffs temporarily raise movement speed, jump height and
damage dealt. Each shows an aura around the character and expires on its own timer.

**How it works.** `SpeedBuff`, `JumpBuff` and `StrengthBuff` implement `IBuffEffect`, which exposes a
family of query methods — `getSpeedMultiplier()`, `getJumpMultiplier()`, `getDamageMultiplier()` — with
neutral defaults. A buff overrides only the one it cares about. `Player` never asks "am I sped up?"; it
asks `BuffManager` for the current total multiplier and multiplies its base stat by it. Because the
buff returns a multiplier rather than mutating the player's speed, an expiring buff cannot leave the
player permanently fast — a class of bug that additive mutation invites.

**Related files.** `include/entity/BuffEffects/SpeedBuff.h` · `JumpBuff.h` · `StrengthBuff.h` · `include/entity/BuffEffects/IBuffEffect.h` · `src/entity/Player/BuffManager.cpp`

---

## F056 — Protective buffs: shield and healing

**Subsystem:** Buffs

**What it does.** The shield grants outright immunity for its duration. The healing buff restores health
over time rather than instantly, so it rewards surviving rather than tanking.

**How it works.** `ShieldBuff` overrides `isInvincible()` to return true; `Player` consults
`BuffManager::isInvincible()` in the same place it consults its own invincibility-frame timer, so both
sources of immunity are handled by one check. `HealBuff` uses the `onApply`/`onRemove` hooks on
`IBuffEffect` plus a per-tick contribution during `update`, which is the mechanism that lets a buff have
an effect other than a stat multiplier without widening the interface for everyone else.

**Related files.** `include/entity/BuffEffects/ShieldBuff.h` · `HealBuff.h` · `src/entity/Player/BuffManager.cpp` · `src/entity/Player/Player.cpp`

---

## F057 — Utility buffs: invisibility, gold magnet, time stop and mystery

**Subsystem:** Buffs

**What it does.** Invisibility makes enemies stop targeting the player. The gold magnet pulls nearby
coins toward them. Time stop freezes every enemy in the room. The mystery buff rolls into one of the
others at the moment it is consumed, so it is a gamble.

**How it works.** Each exposes its capability as a query on `IBuffEffect` — `isInvisible()`,
`hasGoldMagnet()`, `canTimeStop()` — and the systems that care poll `BuffManager` rather than being
notified. Enemy targeting (F045) skips invisible players; `Coin` checks the magnet flag and steers toward
the player when set; the level skips enemy `Update` calls while time stop is active. `RandomBuff` is the
interesting one: rather than being a fourth capability, it selects a concrete buff at apply time and
delegates to it entirely, so the randomness lives in one class and every other system sees an ordinary
buff.

**Related files.** `include/entity/BuffEffects/InvisiBuff.h` · `GoldMagnetBuff.h` · `TimeStopBuff.h` · `RandomBuff.h` · `src/entity/Player/BuffManager.cpp`

---

## F058 — Additive buff stacking

**Subsystem:** Buffs (architecture)

**What it does.** Buffs combine. Picking up a second speed buff while one is running makes the player
faster still, and each expires on its own schedule rather than the newer one cancelling the older. A
player can be simultaneously fast, strong, shielded and invisible.

**How it works.** `BuffManager` holds a `vector<ActiveBuff>`, each pairing an `IBuffEffect` with its
remaining time. Queries **fold across the whole vector**: `getTotalSpeedMultiplier()` multiplies every
active contribution, `isInvincible()` returns true if any active buff claims it. Expiry removes one entry
without disturbing the others. This is worth stating precisely, because it is a place the project
deliberately declined a pattern: Strategy selects *one* algorithm at a time, and this selects *none* — it
aggregates all of them. Calling it Strategy would be a mislabel; the accurate description is a
polymorphic modifier collection, and the design report analyses why the distinction matters.

**Related files.** `src/entity/Player/BuffManager.cpp` · `include/entity/Player/BuffManager.h` · `include/entity/BuffEffects/IBuffEffect.h`

---
---

# 10. Shop and economy

## F059 — In-world shop

**Subsystem:** Shop

**What it does.** A shop stands inside the level as a physical building. Walking up to it opens a
purchase interface; walking away closes it. It is a place in the world, not a menu entry.

**How it works.** `ShopAsset` is a `BaseItem` that detects player proximity and opens `ShopUIPanel`,
passing a pointer to the specific player who approached — so in co-op the correct player's coins are
spent. The pointer is explicitly non-owning: the shop panel observes a player it does not control and
whose lifetime it does not manage. A bug found and fixed during this work: `ShopAsset` originally loaded
its artwork through a hard-coded absolute path from one developer's machine, which meant the shop was
invisible on the other member's computer and would have been invisible on the grader's.

**Related files.** `src/entity/Item/ShopAsset.cpp` · `src/ui/ShopUIPanel.cpp` · `include/ui/ShopUIPanel.h`

---

## F060 — Purchase interface

**Subsystem:** Shop and interface

**What it does.** The shop shows a scrollable catalogue of items with artwork, prices, hover highlighting
and buy buttons. While it is open the world behind it pauses, so the player is not attacked while
shopping.

**How it works.** `ShopUIPanel` holds a `vector<ShopItemDef>` describing what is for sale and renders it
against a scroll offset, clipping rows to the panel bounds. Pausing is achieved by the level checking the
panel's `isOpen_` before running its entity update pass — a single guard at the level, rather than a
"paused" flag every entity must respect. The panel follows the same four-phase discipline as every other
widget: `update(dt, mouse, pressed)` records interaction, and the purchase is executed in a later phase.

**Related files.** `src/ui/ShopUIPanel.cpp` · `include/ui/ShopUIPanel.h` · `src/states/BaseLevelState.cpp`

---

## F061 — Coin-based purchasing with feedback

**Subsystem:** Shop and economy

**What it does.** Buying deducts the price from the shared coin total and gives the item. If the player
cannot afford it, the attempt reports insufficient funds instead of silently failing.

**How it works.** The purchase path reads the shared `PartyInventory` — the same object the HUD displays
and the save system serialises — so there is exactly one coin total in the program and no possibility of
the shop and the HUD disagreeing. A successful purchase places the item into the buyer's stored slot,
which means it also obeys the one-slot swap rule (F052): buying a bomb while holding a potion drops the
potion. Feedback is delivered through the same floating-text mechanism used by combat, so the shop
needed no message system of its own.

**Related files.** `src/ui/ShopUIPanel.cpp` · `src/entity/Player/Player.cpp` · `include/save/InventorySaveData.h`

---
---

# 11. Save, checkpoint and progression

## F062 — Multi-version saving

**Subsystem:** Save system

**What it does.** Saves are never overwritten. Every checkpoint reached writes a **new** version, so a
player accumulates a history of a run and can return to any earlier point — including going back before
a decision they regret. Each world keeps its own independent history.

**How it works.** `FileSaveRepository` writes to `saves/world0X/versionY.json`, allocating the next free
version number for that world. Writes are **atomic**: the file is written to a temporary path and then
moved into place, so a crash or a power loss during a write cannot leave a half-written save that would
be loaded as valid on the next launch. The repository is reached only through the `ISaveRepository`
interface, which is what allows the whole storage mechanism to be swapped or stubbed in tests without
touching any gameplay code.

**Related files.** `src/save/FileSaveRepository.cpp` · `include/save/ISaveRepository.h` · `include/save/FileSaveRepository.h`

---

## F063 — Complete state capture

**Subsystem:** Save system

**What it does.** A save is not just a position. It records the character being played, their health,
mana and breath, the coins and keys the party holds, the item being carried, which room of the world the
player is in, which enemies have already been defeated, and which items have changed from their starting
state. Loading a save gives back the run as it was, not an approximation of it.

**How it works.** The snapshot is a tree of plain data structures with no behaviour and no dependencies:
`GameSaveData` aggregates `PlayerSaveData`, `InventorySaveData`, `LevelSaveData` and `SaveMetaData`.
Keeping them free of logic is what makes them safe to serialise, safe to copy and safe to compare, and it
means the save layer has no knowledge of `Player`, `Mob` or `TileMap` — the level converts its live
objects into these structs and back. `LevelSaveData` is the interesting member: it holds the current room
name plus the sets of defeated-enemy and changed-item identifiers that make F067 possible.

**Related files.** `include/save/GameSaveData.h` · `PlayerSaveData.h` · `InventorySaveData.h` · `LevelSaveData.h` · `SaveMetaData.h`

---

## F064 — Save previews and deletion

**Subsystem:** Save system

**What it does.** The load list shows a readable summary of each save — character, room, coins, health,
playtime, timestamp — so the player can tell their saves apart before loading one. Unwanted saves can be
deleted from the same screen.

**How it works.** `SaveSlotInfo` is a small summary struct returned by the repository's listing call.
Building a list of twenty saves does not require deserialising twenty complete world snapshots, which is
what keeps the browser instant. Deletion goes through the repository interface too, so the UI never
constructs a file path or calls a filesystem function — the panel knows only that it holds a slot and
that the repository can remove it. Robustness was tested deliberately: corrupt JSON, a zero tile size,
negative and absurd map dimensions, and out-of-range keys are all rejected rather than crashing, and a
`.value()` accessor is used instead of `operator[]` because `nlohmann::json`'s const `operator[]`
dereferences `end()` under `NDEBUG` rather than throwing.

**Related files.** `include/save/SaveSlotInfo.h` · `src/save/FileSaveRepository.cpp` · `src/ui/SaveVersionPanel.cpp` · `src/save/JsonSaveSerializer.cpp`

---

## F065 — Checkpoints write saves

**Subsystem:** Save system and progression

**What it does.** Reaching a flag is the moment of saving. The player always knows their progress is
secured, and the save list reads as a record of how far each attempt got.

**How it works.** `BaseLevelState` assembles a `GameSaveData` from the live world — reading player stats,
the shared inventory, the current room name and the accumulated sets of defeated enemies and changed
items — and passes it to the repository. Assembly happens in the level rather than in the save system
because the level is the only object that can see all of those parts; the save system's job is storage,
not gathering. `SaveMetaData` stamps the version with a timestamp, formatted through `localtime_s` on
Windows so the code builds under both MSVC and MinGW.

**Related files.** `src/states/BaseLevelState.cpp` · `src/entity/Item/Flag.cpp` · `src/save/FileSaveRepository.cpp` · `include/save/SaveMetaData.h`

---

## F066 — Respawn restores world state

**Subsystem:** Progression

**What it does.** Dying does not merely move the player back to the flag. It returns the *world* to how
it was at that checkpoint: enemies killed after the checkpoint are alive again, chests opened after it
are shut again. The player replays the same challenge, not a hollowed-out version of it.

**How it works.** The snapshot taken at the checkpoint is retained in memory for exactly this purpose.
On respawn, `BaseLevelState` reapplies it: the room is rebuilt, entities are reconstructed through the
factories, and the recorded defeated/changed sets are replayed onto them. Rebuilding from the same data
path used by loading a save file means there is one restore implementation rather than two, and any bug
in it shows up in both places at once instead of hiding in the rarer one.

**Related files.** `src/states/BaseLevelState.cpp` · `include/save/LevelSaveData.h` · `src/entity/EnemyFactory.cpp` · `src/entity/Item/ItemFactory.cpp`

---

## F067 — Defeated enemies and changed items persist

**Subsystem:** Progression

**What it does.** Enemies stay dead. Leaving a room and coming back does not repopulate it, and neither
does reloading a save. Opened chests stay open, collected coins stay collected, struck blocks stay
struck, unlocked doors stay unlocked.

**How it works.** This depends on entities having **stable identity**, which is what LDtk's `iid` field
provides: every entity placed in the editor carries a unique identifier that survives being saved,
reloaded and reconstructed. `LevelSaveData` holds a set of defeated `iid`s and a map of changed item
states keyed by `iid`. When a room is built, the factories consult those collections and skip or adjust
accordingly. Without stable identity this feature is not implementable — position-based matching breaks
the moment two identical enemies stand near each other — which is one of the strongest practical
arguments for using a real level format rather than an ad-hoc one.

**Related files.** `include/save/LevelSaveData.h` · `include/entity/Item/ItemState.h` · `src/states/BaseLevelState.cpp` · `src/environment/TileMap.cpp`

---

## F068 — Playtime tracking

**Subsystem:** Progression

**What it does.** Each save shows how long that run has been played, so a player returning after a break
can tell a serious attempt from an abandoned one.

**How it works.** `BaseLevelState` accumulates elapsed simulation time while the level is actually
running, and specifically **not** while a menu, the shop panel or a cutscene is holding the world.
Because it accumulates fixed timesteps rather than wall-clock time, the number reflects time spent
playing rather than time the window was open. The total is carried in `SaveMetaData`, so it rides along
with the version it belongs to and is displayed by the save browser without opening the full snapshot.

**Related files.** `src/states/BaseLevelState.cpp` · `include/save/SaveMetaData.h` · `src/ui/SaveVersionPanel.cpp`

---
---

# 12. Level, world and camera

## F069 — LDtk level loading

**Subsystem:** Environment

**What it does.** Levels are authored in **LDtk**, a real level-design tool, and loaded straight from
the `.ldtk` project file. Designers get a proper editor with layers, tilesets, auto-layers and entity
fields; the game gets the tile layers, the collision grid, the entity placements and the room graph
without any intermediate export step.

**How it works.** `TileMap::LoadLDtkMap(path, levelName)` parses the LDtk JSON with nlohmann/json and
extracts four things: the visual tile layers, the integer grid that becomes the `CollisionType` array,
the entity instances with their positions, custom fields and stable `iid`s, and the level's world
coordinates and neighbour list. Entities are not constructed here — they are returned as
`LDtkEntityData` records and handed to the factories, which keeps `TileMap` free of any knowledge of
`Coin`, `Mob` or `Flag`. A world scale is derived from the source tile size so that maps drawn at 16px
and maps drawn at 32px both play at a consistent physical scale.

**Related files.** `src/environment/TileMap.cpp` · `include/environment/TileMap.h` · `assets/maps/map01/` … `map06/`

---

## F070 — Seamless room transitions

**Subsystem:** Environment and progression

**What it does.** Worlds are made of many connected rooms — world 6 has 24, world 4 has 19. Walking off
the edge of one loads the next and places the player correctly on the far side, with no loading screen
and with everything already done in the previous room preserved.

**How it works.** LDtk records each level's position in a world grid and its neighbours by direction.
`TileMap::GetNeighbour(dir, x, y)` resolves which room lies that way, and `BaseLevelState` swaps the map,
rebuilds the entities through the factories, and places the player at the matching edge. Preservation is
the hard part and is handled by carrying the accumulated defeated-enemy and changed-item sets across the
swap (F067) — the new room is built *filtered* by that state rather than built fresh and then corrected.
The shared setup work was extracted into `initWorldFromLoadedMap()` so that the six world screens and
the custom-map path all construct their world identically instead of each repeating it.

**Related files.** `src/states/BaseLevelState.cpp` · `src/environment/TileMap.cpp` · `include/save/LevelSaveData.h`

---

## F071 — World registry

**Subsystem:** Screens (architecture)

**What it does.** Six worlds are selectable, each with its own map file, its own save folder and its own
level screen. Adding a seventh is a single registry entry.

**How it works.** `WorldCatalog` holds a table of `WorldDescriptor` records, each carrying a world index,
a display name, a `.ldtk` path, a save-folder name and a factory function that constructs the right
level screen. Before this existed, the same `switch (worldIndex)` was duplicated in the map-selection
screen, the load path and the save path — three places that could disagree, and did. Replacing them with
one lookup removed that class of bug entirely and is the reason the new-game and load-game routes cannot
send the player to different worlds for the same click.

**Related files.** `src/states/WorldCatalog.cpp` · `include/states/WorldCatalog.h` · `include/states/WorldDescriptor.h` · `src/states/MapSelectionState.cpp`

---

## F072 — Pre-rendered background layer

**Subsystem:** Environment and rendering

**What it does.** Large rooms draw at full speed. A map thousands of tiles across does not cost thousands
of draw calls per frame.

**How it works.** When a level loads, `TileMap` composites the entire static background and terrain into
a single `RenderTexture2D` once. Every subsequent frame draws that one texture with the camera transform
applied, rather than iterating the tile grid. Only genuinely dynamic content — entities, effects,
projectiles — is drawn per frame. The trade is memory for time, and it is the right trade here because
the static layers never change during play; the moment they do change, in the map editor, the editor
uses a different path that redraws the working grid directly.

**Related files.** `src/environment/TileMap.cpp` · `include/environment/TileMap.h`

---

## F073 — Follow camera for one and two players

**Subsystem:** Camera

**What it does.** The camera follows the player smoothly, with a dead zone so small movements do not
jitter the view, look-ahead that leads in the direction of travel, and clamping so it never shows the
void outside the map. In two-player modes it frames both players at once, keeping both on screen as they
separate.

**How it works.** `CameraFollowMode` implements `ICameraMode` and holds the follow logic: it eases
toward the target rather than snapping, offsets by a fraction of the target's velocity to reveal what is
ahead, and clamps the result against the map dimensions. The two-player variant takes both positions and
targets their midpoint. Clamping against real map bounds is why the camera behaves correctly in the
long horizontal rooms of world 3 and the tall vertical shafts of world 4 without per-room tuning.

**Related files.** `src/environment/CameraFollowMode.cpp` · `src/environment/MapCamera.cpp` · `include/environment/ICameraMode.h`

---

## F074 — Queued cinematic camera modes

**Subsystem:** Camera

**What it does.** The camera can be scripted. A cutscene pans it to a boss, zooms in, holds, and returns
control — and an interrupted cutscene never leaves the camera stranded looking at a wall.

**How it works.** `MapCamera` owns a `unique_ptr<ICameraMode> currentMode` and a
`queue<unique_ptr<ICameraMode>>`. Three modes implement the interface: `CameraFollowMode`,
`CameraPanMode` (with a duration and an easing type) and `CameraZoomMode`. A cutscene pushes the shots
it wants and finally pushes a follow mode; each mode reports `isFinished()` and the camera advances to
the next. The structural insight is that **following is not a special default state** — it is simply
another entry in the queue. That is exactly why an aborted cutscene recovers correctly: returning to
normal is a queued item, not a flag someone must remember to reset.

**Related files.** `src/environment/MapCamera.cpp` · `CameraPanMode.cpp` · `CameraZoomMode.cpp` · `include/environment/ICameraMode.h`

---
---

# 13. Map editor

## F075 — In-game level editor

**Subsystem:** Editor

**What it does.** A complete level editor is built into the game and reachable from the main menu. The
canvas is a grid that can be panned and zoomed, and a translucent ghost of whatever is about to be placed
follows the cursor so the player can see the result before committing.

**How it works.** `MapEditorState` is an ordinary `GameState` following the same four-phase rule as
gameplay, which means the editor gets the fixed timestep and the deferred transitions for free.
`EditorCamera` handles pan and zoom and converts between screen and grid coordinates — one conversion
used by every tool, rather than each tool doing its own arithmetic. The ghost is drawn by the active tool
through `IEditorTool::renderGhost(gx, gy, cellRect, data)`, so a tile tool previews a tile and an entity
tool previews an entity without the editor screen knowing the difference.

**Related files.** `src/states/MapEditorState.cpp` · `src/editor/EditorCamera.cpp` · `include/editor/IEditorTool.h`

---

## F076 — Categorised block palette

**Subsystem:** Editor

**What it does.** Terrain blocks are grouped into browsable categories, and each category offers the
visual variants available for it, drawn from the tilesets of five different worlds. The player builds
with the same art the campaign is built from.

**How it works.** `EditorBlockRegistry` holds `EditorBlockDef` records describing every placeable block:
its category, its source tileset, its tile indices and its collision type. `CategoryPanel` renders the
categories and `BlockVariantPanel` renders the variants of the selected one. Icons are fitted into their
slots by `IconFit`, a small helper that computes the correct source rectangle and scale so that tiles
authored at different sizes across five tilesets all appear consistently in the palette.
`EditorTextureCache` keeps the tilesets loaded once for the editor session.

**Related files.** `src/editor/EditorBlockRegistry.cpp` · `src/editor/CategoryPanel.cpp` · `src/editor/BlockVariantPanel.cpp` · `include/editor/IconFit.h` · `src/editor/EditorTextureCache.cpp`

---

## F077 — Entity, enemy and boss palette

**Subsystem:** Editor

**What it does.** Beyond terrain, the editor places real gameplay objects: player spawn points, coins,
keys, doors, lucky blocks, chests, flags, bombs, buff pickups and springs — plus **all nine enemy types
and all six bosses**. A custom map can be a genuine level with a boss fight at the end.

**How it works.** `EntityPalette` is driven by `EntityDef` records, and the entity types it offers are the
same identifiers the gameplay factories understand. That is the whole trick: a placed entity is stored as
a `CustomEntityData` record holding a type identifier, a grid position and its fields, and at play time
those records go to `ItemFactory` and `EnemyFactory` — the identical factories the LDtk path uses. No
enemy needed a second construction path to become placeable.

**Related files.** `src/editor/EntityPalette.cpp` · `include/editor/EntityDef.h` · `include/editor/CustomEntityData.h` · `src/entity/EnemyFactory.cpp` · `src/entity/Item/ItemFactory.cpp`

---

## F078 — Rule-based auto-tiling

**Subsystem:** Editor

**What it does.** The player paints solid terrain and the editor chooses the correct artwork for it —
grass on top, dirt beneath, the right corner piece at each corner, the right edge along each side, with
randomised variants so a long wall does not look repetitive. Painting a shape produces a finished-looking
piece of level rather than a block of identical squares.

**How it works.** `AutoTiler` implements LDtk's own auto-layer rule model, using rule groups exported
from the LDtk project into `assets/maps/extracted_rules.json`. Each rule is a pattern over the
neighbourhood of a cell — which surrounding cells must be filled and which must be empty — together with
the tiles to place when it matches, an optional random chance and optional horizontal or vertical flips.
When a cell changes, the tiler re-evaluates that cell and its neighbours in rule-group priority order.
Reusing LDtk's real rule format rather than inventing a simpler one is what makes editor output visually
indistinguishable from the authored campaign maps.

**Related files.** `src/editor/AutoTiler.cpp` · `include/editor/AutoTiler.h` · `assets/maps/extracted_rules.json`

---

## F079 — Erase tool

**Subsystem:** Editor

**What it does.** A dedicated eraser removes both tiles and entities, and it is always available on the
right mouse button no matter which tool is currently selected — so correcting a mistake never costs a
trip to the toolbar.

**How it works.** `EraseTool` implements `IEditorTool` exactly as the placement tools do, and
`EditorToolManager` holds the active tool plus a permanent right-button binding to the eraser. Erasing a
tile triggers the auto-tiler to re-evaluate the surrounding cells, so removing a block from the middle of
a wall correctly reopens the edge artwork around the hole. `IEditorTool` returns a `bool` from `onPress`
and `onDrag` indicating whether the map actually changed, which is what lets the undo system record one
entry per meaningful action rather than one per frame of a drag.

**Related files.** `src/editor/EraseTool.cpp` · `src/editor/EditorToolManager.cpp` · `include/editor/IEditorTool.h` · `include/editor/EditorToolType.h`

---

## F080 — Multi-step undo and redo

**Subsystem:** Editor

**What it does.** Editing actions can be reversed and reapplied, up to fifty steps back — including map
resizes, which are the most destructive operation the editor offers. Experimenting is safe.

**How it works.** `UndoRedoStack` stores **snapshots** of the whole `CustomMapData` rather than
descriptions of individual edits. Each completed action pushes a copy of the map onto the undo stack and
clears the redo stack; undo moves the current state to the redo stack and restores the previous snapshot.
The choice of snapshots over inverse operations is deliberate: auto-tiling means a single click can
modify a dozen cells in ways that are awkward to invert exactly, whereas a snapshot restores all of them
correctly by construction. A depth cap of fifty bounds the memory cost, which is the price of the
simplicity.

**Related files.** `src/editor/UndoRedoStack.cpp` · `include/editor/UndoRedoStack.h` · `include/editor/CustomMapData.h`

---

## F081 — Drag-to-resize map

**Subsystem:** Editor

**What it does.** Handles on all four edges of the map grow or shrink it by dragging, with the result
visible live. A map that turns out too small does not have to be rebuilt.

**How it works.** `EditorMapResizer` draws the four handles and translates a drag into a new width,
height and origin offset. Growing from the top or left is the subtle case, because every existing tile
and entity must shift to keep its position relative to the content: the resizer rebuilds the grid at the
new dimensions and copies the old contents in at the corrected offset, adjusting entity grid coordinates
to match. Limits are enforced so the map cannot be dragged to zero or to a size that would exhaust
memory, and the completed resize is pushed to the undo stack as one action.

**Related files.** `src/editor/EditorMapResizer.cpp` · `include/editor/EditorMapResizer.h` · `src/editor/UndoRedoStack.cpp`

---

## F082 — Save, load and instant test play

**Subsystem:** Editor and persistence

**What it does.** A map in progress can be saved to a numbered slot, reopened later for more editing,
and — the important one — **played immediately** from the editor as a real level, with real enemies, real
physics and real combat. Finishing the test returns to the editor with the map still loaded.

**How it works.** `CustomMapSerializer` writes `CustomMapData` to JSON and reads it back;
`EditorSaveLoadUI` provides the slot browser with a `SaveLoadMode` distinguishing save from load so one
panel serves both. Test play constructs a `BaseLevelState` from the in-memory `CustomMapData` through
`TileMap::LoadCustomMap`, which produces exactly the same internal model that `LoadLDtkMap` produces —
which is why a custom map is a genuine level rather than a limited preview. `CustomMapValidator` runs
first and reports what is missing (no player spawn, no exit) instead of launching a level that cannot be
completed.

**Related files.** `src/editor/CustomMapSerializer.cpp` · `src/editor/EditorSaveLoadUI.cpp` · `include/editor/CustomMapValidator.h` · `include/editor/SaveLoadMode.h` · `src/environment/TileMap.cpp`

---
---

# 14. Cutscene and dialogue

## F083 — Scripted one-shot cutscenes

**Subsystem:** Cutscenes

**What it does.** Walking into certain places takes control away, moves the camera to something the
player should see, plays a conversation, and hands control back. A cutscene that has already played does
not replay when the player walks through the same spot again.

**How it works.** `CutsceneManager` runs a four-phase sequence — take control, move camera, run dialogue,
restore — and coordinates three subsystems that otherwise know nothing about each other: it suppresses
player input, pushes camera modes onto `MapCamera`'s queue (F074), and drives `DialogueBox`. Because the
camera work goes through the queue and ends by pushing a follow mode, an interrupted cutscene still
restores the camera correctly. Played cutscenes are recorded by identifier in the manager, which is what
makes them one-shot.

**Related files.** `src/cutscene/CutsceneManager.cpp` · `include/cutscene/CutsceneScript.h` · `src/environment/MapCamera.cpp`

---

## F084 — Cutscene triggers authored in the level

**Subsystem:** Cutscenes and level data

**What it does.** Cutscenes are placed by the level designer, not written into code. A trigger volume
drawn in LDtk carries which dialogue to play, where the camera should look, how far to zoom and how long
to hold.

**How it works.** `CutsceneTrigger` is constructed from an `LDtkEntityData` record and reads its
parameters out of the entity's `fieldInstances` — the custom fields LDtk lets a designer attach to a
placed entity. Overlapping it fires the cutscene by name. This is the same data channel that gives a
spring its direction (F049) and an enemy its type, which means adding a new authored parameter to
anything in the game is an LDtk field definition rather than a new file format.

**Related files.** `src/cutscene/CutsceneTrigger.cpp` · `include/cutscene/CutsceneTrigger.h` · `include/environment/TileMap.h`

---

## F085 — Typewriter dialogue with speaker registry

**Subsystem:** Dialogue

**What it does.** Conversations reveal character by character at a readable pace, showing who is
speaking, and advance on a keypress. Pressing again during the reveal completes the line instantly rather
than skipping it.

**How it works.** `DialogueRegistry` loads all dialogue from JSON at startup through `DialogueLoader`
and stores it as `DialogueSequence` objects keyed by identifier — each a list of `DialogueLine`
records carrying a speaker and a line — so a cutscene references a conversation by name and never
carries text in code. `DialogueBox` owns the presentation: a per-line reveal speed, a character
cursor advanced by elapsed time, the speaker's name, and the advance/complete input rule. Separating the
registry from the box means the same dialogue can be triggered from a cutscene, a boss intro or a level
event with one implementation, and translating the game would be a matter of swapping one JSON file.

**Related files.** `src/dialogue/DialogueBox.cpp` · `src/dialogue/DialogueRegistry.cpp` · `src/dialogue/DialogueLoader.cpp` · `include/dialogue/DialogueData.h`

---
---

# 15. Settings and controls

## F086 — Full key rebinding for two players

**Subsystem:** Input and settings

**What it does.** Every action — move, jump, crouch, the four attacks, the special, dash, block and item
use — can be reassigned to any key, **independently for player 1 and player 2**. That is what makes local
co-op on a single keyboard workable: the two players configure halves of the keyboard that suit them.
Defaults can be restored per player or for both at once.

**How it works.** `SettingsManager` holds two binding maps from action to key code. `InputHandler` is the
only code in the project that reads the keyboard for gameplay; everything else asks for an *action*, so
no state, entity or skill contains a `KEY_` constant and rebinding cannot miss a site. The settings UI is
built from `ButtonGroup::AddKeybind(label, action, currentKey, defaultKey, isP1)` rows, each of which
enters a capture mode on click and writes the next key pressed. Because bindings are data consulted at
read time rather than compiled in, a rebind takes effect on the very next frame.

**Related files.** `src/core/SettingsManager.cpp` · `include/core/SettingsManager.h` · `src/core/InputHandler.cpp` · `src/ui/ButtonGroup.cpp`

---

## F087 — Audio volume settings

**Subsystem:** Settings

**What it does.** Four sliders — master, music, player sound effects and enemy sound effects — are
available both from the main menu and from the in-game settings panel, and the same values are shown in
both places.

**How it works.** `ButtonGroup::AddSlider(label, getter, setter)` takes a pair of `std::function`
callbacks rather than a value. The slider widget therefore never knows what it is editing: it renders a
track, reads the current value through the getter and writes through the setter on drag. Binding those
callbacks to `SettingsManager::GetMasterVolume` / `SetMasterVolume` and friends is what allows the
main-menu settings screen and `IngameSettingsPanel` to present the same four controls from the same
source of truth with no duplicated state. Each setter clamps to `[0,1]` and persists immediately.

**Related files.** `src/ui/ButtonGroup.cpp` · `src/ui/IngameSettingsPanel.cpp` · `src/core/SettingsManager.cpp`

---

## F088 — Settings persistence

**Subsystem:** Settings

**What it does.** Every preference — both key maps, all four volumes and the menu options — is written
to disk the moment it changes and restored on the next launch. Nothing is lost by closing the game, and
nothing needs an explicit "apply".

**How it works.** `SettingsManager` is a singleton whose setters each call `SaveToFile()`, writing
`assets/config/session.json`. Saving on change rather than on exit means a crash cannot lose the player's
configuration, which matters more than the tiny write cost. Loading happens once at construction, and a
missing or malformed file falls back to defaults rather than failing to start — the game must be
runnable on a machine that has never run it before, including the grader's.

**Related files.** `src/core/SettingsManager.cpp` · `include/core/SettingsManager.h` · `assets/config/session.json`

---
---

# 16. Visual presentation and interface

## F089 — Player status HUD

**Subsystem:** Interface

**What it does.** The player always sees their character portrait, health bar, mana bar, breath meter,
coin and key totals, and the item currently held. In two-player modes each player gets their own bars on
their own side of the screen while the pooled coin and key totals appear once.

**How it works.** `PlayerHUD` is a stateless renderer: a single static `render(p1, p2, inventory)` call
that reads live objects and draws, owning nothing. Passing a null second player switches it to the
one-player layout, so there is one function rather than two parallel HUDs that could drift apart. That
the shared totals appear once falls directly out of the shared `PartyInventory` (F046) — the HUD is
showing one object because there genuinely is only one.

**Related files.** `src/ui/PlayerHUD.cpp` · `include/ui/PlayerHUD.h` · `src/states/BaseLevelState.cpp`

---

## F090 — Iris screen transitions

**Subsystem:** Interface

**What it does.** Screens close and open with a circular wipe that contracts to a point and expands
again, with a soft feathered edge, rather than cutting abruptly.

**How it works.** `IrisTransition` implements `ITransition` (`Start(bool out)`, `Update`, `Render`,
`IsFinished`). It draws an expanding or contracting masked overlay with a configurable radius, speed,
feather size and colour. Screens drive it in the outgoing direction, wait for `IsFinished()`, and only
then enqueue their state change — which means the transition also functions as the input lock during a
screen change, preventing a double-click from queueing two transitions. The interface exists so other
wipe styles can be added without any screen changing.

**Related files.** `src/ui/transitions/IrisTransition.cpp` · `include/ui/transitions/ITransition.h` · `src/states/CharacterSelectionState.cpp`

---

## F091 — Animated GIF backgrounds and world objects

**Subsystem:** Infrastructure and presentation

**What it does.** Genuine animated GIFs play in the game — the character-selection background is an
animated scene, and the level exit gate is an animated object standing in the world.

**How it works.** `GifAnimation` wraps raylib's `LoadImageAnim` to decode every frame of a GIF into one
CPU-side image, uploads the first frame to a texture, and then advances by calling `UpdateTexture` with
the correct frame offset — one texture reused rather than one texture per frame. It accumulates elapsed
time and can advance several frames in one update, so a frame spike does not desynchronise the animation.
Transparency required care: re-encoding `gate.gif` during development destroyed its alpha channel and
turned 73% of the image opaque black, because the palette conversion dropped the transparency index. The
fix was to build the palette from opaque pixels only and reserve one index for transparency, verified by
comparing every pixel against the original.

**Related files.** `src/infrastructure/GifAnimation.cpp` · `include/infrastructure/GifAnimation.h` · `src/states/CharacterSelectionState.cpp` · `src/entity/Item/EndgameAsset.cpp`

---

## F092 — Sprite-atlas animation with shared registry

**Subsystem:** Presentation

**What it does.** Items animate — coins spin, chests glint, buffs pulse — and a room containing a hundred
coins costs no more texture memory than a room containing one.

**How it works.** `ItemAtlasRegistry` owns one atlas texture per item family. `AtlasAnimation` holds only
what differs between instances: which atlas, the current frame index, the elapsed time and the frame
rectangle. The texture itself — the large, expensive part — is shared. This is Flyweight in its textbook
form: intrinsic state (the atlas, the frame layout) is shared and stored once; extrinsic state (position,
current frame, tint) stays with the instance. The concrete payoff is that adding fifty coins to a room
costs fifty small structs, not fifty texture uploads.

**Related files.** `src/entity/Item/ItemAtlasRegistry.cpp` · `src/entity/Item/AtlasAnimation.cpp` · `include/entity/Item/ItemAtlasRegistry.h`

---

## F093 — Explosions and curved projectile trails

**Subsystem:** Presentation and combat

**What it does.** Explosions bloom outward with animation matched to their damage radius, and projectiles
can travel along a visible sine curve rather than a flat line — so a character's ranged attack reads as
theirs at a glance.

**How it works.** `Explosion` renders its animation scaled to the radius declared in `ExplosionConfig`,
so the visual and the damage area agree by construction rather than by two numbers someone has to keep in
sync. `Fireball` optionally modulates its position perpendicular to its direction of travel by a sine of
elapsed time, with amplitude and frequency from `FireballConfig`; the collision hitbox follows the
rendered position, so a curving projectile genuinely hits where it appears to be.

**Related files.** `src/entity/Explosion.cpp` · `src/entity/Fireball.cpp` · `include/entity/ExplosionConfig.h` · `include/entity/FireballConfig.h`

---

## F094 — Status overlays and buff auras

**Subsystem:** Presentation

**What it does.** Every temporary state is visible. Poison and burning draw animated overlays on the
affected character; active buffs draw a looping aura around the player; invincibility flashes. A player
can read their own condition without consulting a bar.

**How it works.** Each `IEffect` renders its overlay inside the affected entity's own render pass, so
an overlay follows the character through every animation without separate tracking. `BuffManager::render`
walks the active buff list and lets each buff draw itself via `IBuffEffect::render(Player&, float)` —
which is why several simultaneous buffs produce several stacked auras rather than one generic effect, and
why adding a buff with a distinctive look requires no change to the player or the renderer.

**Related files.** `src/entity/Effects.cpp` · `src/entity/Player/BuffManager.cpp` · `include/entity/BuffEffects/IBuffEffect.h`

---

## F095 — Reusable interface toolkit

**Subsystem:** Interface (architecture)

**What it does.** Every panel in the game — main menu, settings, world selection, save browser, shop,
pause, editor — is built from one shared widget set with consistent behaviour: buttons with hover and
pressed artwork, tabs, sliders, key-binding rows, scrolling lists and click handling.

**How it works.** `ButtonGroup` hosts the composite widgets (buttons, tabs, keybind rows, sliders,
scrolling); `PanelButton` is the standalone image button used by the game-flow panels; `Button` provides
a fluent-configured simple button (`setPosition().setSize().setLabel().setOnClick()`); `IMenuPanel` is the
common panel contract. All of them respect the four-phase rule — `HandleInput` records, a later phase
acts — which is why a click can never trigger a state change in the middle of the input pass. All of them
lay out through `UIScaler`, which is why a single design-resolution change propagated to every screen at
once.

**Related files.** `src/ui/ButtonGroup.cpp` · `src/ui/PanelButton.cpp` · `src/ui/Buttons.cpp` · `include/ui/IMenuPanel.h` · `src/ui/UIScaler.cpp`

---
---

# 17. Game modes

## F096 — Single-player story mode

**Subsystem:** Game modes

**What it does.** The main campaign. Choose a world, start fresh or resume a save, choose a character, and
fight through that world's rooms — 63 across the six worlds — collecting coins and keys, opening doors,
defeating enemies, surviving a boss and reaching the exit gate. Progress is secured at checkpoint flags.

**How it works.** The mode is the composition of everything above rather than a subsystem of its own:
`MapSelectionState` → `WorldActionPanel` → either `CharacterSelectionState` or `SaveVersionPanel` →
`LoadingState` → the world's level screen, all of which derive from `BaseLevelState`. The six world
screens exist to hold per-world setup only; the shared behaviour — entity construction, room transitions,
checkpointing, respawn, HUD, camera — lives once in the base class.

**Related files.** `src/states/BaseLevelState.cpp` · `src/states/World01State.cpp` … `World06State.cpp` · `src/states/MapSelectionState.cpp`

---

## F097 — Two-player local co-op

**Subsystem:** Game modes

**What it does.** Two people play the campaign together on one keyboard, with independent controls,
independent health, mana and breath, a shared camera that keeps both in frame, and a pooled inventory —
coins and keys either finds benefit both.

**How it works.** Co-op is not a parallel implementation. `BaseLevelState` holds an optional second
player; the camera switches to its two-target mode; `PlayerHUD` renders the second bar set; enemy target
selection considers both players. Everything else — physics, combat, items, saving — is unchanged, because
those systems were written against "a player" rather than "the player". The single-keyboard problem is
solved entirely by the two independent binding sets in F086.

**Related files.** `src/states/BaseLevelState.cpp` · `src/environment/MapCamera.cpp` · `src/ui/PlayerHUD.cpp` · `src/core/InputHandler.cpp`

---

## F098 — Player-versus-player mode

**Subsystem:** Game modes

**What it does.** Six dedicated arenas where the two players fight each other. A knockout waits for the
loser's death animation to finish and then shows the winner's podium with the victorious character
animating in the centre.

**How it works.** The only change PvP required in the combat system was the faction mask (F027): the
arena builds each player's hitboxes with a mask that includes the other player's faction, and everything
else — damage, defence, knockback, hit-stop, invincibility frames — applies unchanged. The knockout is
sequenced through `PlayerDieState`'s completion signal rather than a health check, which is why the
winner screen appears after the death animation rather than interrupting it. That a whole game mode cost
one bitmask is the clearest practical argument in the project for data-driven rules over type checks.

**Related files.** `src/states/BaseLevelState.cpp` · `src/states/EndgameState.cpp` · `include/entity/EntityFaction.h` · `assets/maps/pvp_map01/` … `pvp_map06/`

---

## F099 — Custom map play

**Subsystem:** Game modes and editor

**What it does.** Maps built in the editor are played as real levels — not previews. They have working
enemies, bosses, items, checkpoints, physics and combat, and depending on how they were built they can be
played solo or as a versus arena.

**How it works.** `TileMap` has two loading entry points, `LoadLDtkMap` and `LoadCustomMap`, that produce
the **same internal model**: the same tile layers, the same `CollisionType` grid, the same entity records.
Everything downstream is therefore identical and no gameplay code knows where the level came from. That
one design decision — a shared data contract with two producers — is what makes the editor a real feature
rather than a toy. `CustomMapValidator` gates the launch, reporting a missing spawn or missing exit before
the player enters an uncompletable level.

**Related files.** `src/environment/TileMap.cpp` · `include/editor/CustomMapData.h` · `src/editor/CustomMapValidator.cpp` · `src/states/BaseLevelState.cpp`

---
---

# Appendix A — Full identifier list

```
F001–F006  Engine and application core             (6)
F007–F013  Screens and navigation                  (7)
F014–F019  Movement and terrain                    (6)
F020–F028  Combat and skills                       (9)
F029–F034  Survival statistics and status          (6)
F035–F038  Playable characters                     (4)
F039–F045  Enemies, bosses and AI                  (7)
F046–F054  Items and interactables                 (9)
F055–F058  Buffs and power-ups                     (4)
F059–F061  Shop and economy                        (3)
F062–F068  Save, checkpoint and progression        (7)
F069–F074  Level, world and camera                 (6)
F075–F082  Map editor                              (8)
F083–F085  Cutscene and dialogue                   (3)
F086–F088  Settings and controls                   (3)
F089–F095  Visual presentation and interface       (7)
F096–F099  Game modes                              (4)
                                          TOTAL = 99
```

Check: 6+7+6+9+6+4+7+9+4+3+7+6+8+3+3+7+4 = 99.

---

# Appendix B — Authored content (not counted as features)

## B.1 Campaign worlds

| World | Map file | Rooms | Representative room names |
|---|---|---|---|
| World 1 | `assets/maps/map01/world01.ldtk` | 1 | `Level_0` |
| World 2 | `assets/maps/map02/world02.ldtk` | 1 | `AutoLayer` |
| World 3 | `assets/maps/map03/world03.ldtk` | 11 | `Top`, `Bottom`, `World_Level_3…10` |
| World 4 | `assets/maps/map04/world04.ldtk` | 19 | `Entrance`, `Large_room`, `Secret_passage`, `Underwater_cove`, `Cliff_cave`, `Water_cave`, `Pit_bottom` |
| World 5 | `assets/maps/map05/world05.ldtk` | 7 | `Level_0…6` |
| World 6 | `assets/maps/map06/world06.ldtk` | 24 | `Entrance`, `Cross_roads`, `Ossuary`, `Sewers1/2`, `Boss_room`, `Shop`, `The_ponds`, `Garden`, `Flooded_rooms`, `Hidden_cave`, `Exit` |
| | **Total** | **63** | |

## B.2 Versus arenas

Six arenas in `assets/maps/pvp_map01/` … `pvp_map06/`.

## B.3 Playable characters

| Character | Health | Mana | Move speed | Jump | Animations |
|---|---|---|---|---|---|
| Goku | 300 | 100 | 300 | −600 | 17 |
| Naruto | 280 | 120 | 320 | −580 | 17 |
| Luffy | 350 | 80 | 280 | −620 | 17 |
| Kakashi | 260 | 130 | 310 | −590 | 17 |
| Sasuke | 270 | 110 | 330 | −600 | 21 |
| Zoro | 320 | 90 | 290 | −580 | 17 |

## B.4 Enemies and bosses

| Enemy | Health | Speed | Detection | Attack range |
|---|---|---|---|---|
| Rat | 30 | 120 | 300 | 35 |
| Bat | 35 | 150 | 350 | 50 |
| Goblin | 40 | 110 | 300 | 40 |
| Slime | 45 | 50 | 200 | 35 |
| Mushroom | 50 | 100 | 100 | 40 |
| Skeleton | 60 | 90 | 250 | 45 |
| Soldier | 80 | 90 | 350 | 150 |
| Tree | 100 | 60 | 200 | 50 |
| Guardian | 200 | 70 | 400 | 60 |
| **Bosses** — Doflam, Franky, Itachi, Sasuke, Shank, Naruto | 1000 each | 80 | 500 | 80–100 |

---

# Appendix C — Project scale

| Metric | Value |
|---|---|
| C++ source files | 328 (192 headers, 136 implementation) |
| Lines of C++ | 23,735 |
| Image assets | 412 PNG, 2 animated GIF |
| Configuration and data files | 31 JSON |
| Level project files | 14 LDtk |
| Abstract interfaces | 9 (`ISkill`, `IBuffEffect`, `IItemUseStrategy`, `IEditorTool`, `ISaveSerializer`, `ISaveRepository`, `ICollisionDetector`, `ICameraMode`, `ITransition`) |
| Distinct behavioural states | 10 player · 7 enemy · 9 boss |

---

*Group 51 — Phạm Đức Minh (25125028) · Lê Tiến Bình (25125007) — September 2026*


