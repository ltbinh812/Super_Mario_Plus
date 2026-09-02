# Super Mario Plus

A 2D action-platformer built with **C++17** and [Raylib](https://www.raylib.com/), featuring 6 playable characters, 6 campaign worlds (63 rooms), 9 enemy types, 6 boss encounters, a co-op/PvP multiplayer mode, and a full in-game level editor.

This project is completely self-contained. It includes precompiled Raylib static libraries and the [nlohmann/json](https://github.com/nlohmann/json) header-only library — meaning you **do not** need a package manager like `vcpkg`, and you don't need to compile any third-party library yourself.

---

## Getting Started

Choose **one** of the following ways to get the project:

**Option A — You already have the project folder (`.SuperMarioPlus`):**
```bash
# Navigate into the project root
cd .SuperMarioPlus
```

**Option B — Clone from GitHub:**
```bash
git clone https://github.com/ltbinh812/Super_Mario_Plus.git
cd Super_Mario_Plus
```

> After either option, make sure your terminal is inside the project folder (the one containing `CMakeLists.txt`) before running any build commands below.

---

## Requirements

- **CMake** 3.19 or higher (required for `CMakePresets.json` support)
- **C++ Compiler** with C++17 support (Windows only):
  - **Visual Studio 2022** (MSVC) — preset: `windows-vs2022`
  - **Visual Studio 2019** (MSVC) — preset: `windows-vs2019`
  - **MinGW-w64** (GCC) — preset: `windows-mingw`

> Make sure `cmake` is available in your terminal (`cmake --version`).  
> If you use MinGW, make sure `mingw32-make` is on your `PATH`.

---

## How to Build and Run

### Option 1: Visual Studio Code (Recommended)

1. Open this project folder in VS Code.
2. Install the **CMake Tools** (`ms-vscode.cmake-tools`) and **C/C++** (`ms-vscode.cpptools`) extensions.
3. When prompted, select a **Configure Preset** matching your compiler:
   - `Windows (VS 2022)` for Visual Studio 2022
   - `Windows (VS 2019)` for Visual Studio 2019
   - `Windows (MinGW)` for MinGW-w64 / GCC
4. Press **F7** (or click **Build**) to compile.
5. Press **Shift+F5** (or click **Launch**) to run the game.

> **Tip:** If you switch between MSVC and MinGW, **delete the `build/` folder first** before reconfiguring.

---

### Option 2: Command Line (Terminal)

> **Important:** All commands below must be run from the **project root directory** (`Super_Mario_Plus/`), i.e. the folder that contains `CMakeLists.txt`. Do **not** run them from inside `src/`, `build/`, or any subfolder.

**Visual Studio 2022 (MSVC):**
```bash
# 1. Configure
cmake --preset windows-vs2022

# 2. Build
cmake --build build --preset windows-vs2022

# 3. Run
.\build\Debug\SuperMarioPlus.exe
```

**Visual Studio 2019 (MSVC):**
```bash
# 1. Configure
cmake --preset windows-vs2019

# 2. Build
cmake --build build --preset windows-vs2019

# 3. Run
.\build\Debug\SuperMarioPlus.exe
```

**MinGW / GCC:**
```bash
# 1. Configure
cmake --preset windows-mingw

# 2. Build
cmake --build build --preset windows-mingw

# 3. Run
.\build\SuperMarioPlus.exe
```

---

## Save Data

Save files and settings are stored in `saves/` at the **project root** (not inside `build/`), so your progress survives clean rebuilds. The folder and its subdirectories are created automatically on first run:

```
saves/
  settings.json          <- key bindings & audio volumes
  world01/ ... world06/  <- per-world checkpoint save slots
  custom_map/            <- maps created with the in-game editor
```

> `saves/` is listed in `.gitignore` and is not tracked by Git. This is intentional — save data is personal and local.

---

## Project Structure

```
SuperMarioPlus/
├── assets/               <- Sprites, maps (.ldtk), audio, fonts, JSON configs
├── include/              <- All header files (mirrors src/ structure)
├── src/
│   ├── core/             <- Game loop, input, state & settings managers
│   ├── entity/           <- Player, enemies, bosses, items, skills, buffs
│   ├── states/           <- All game states (menu, levels, editor, endgame...)
│   ├── environment/      <- TileMap, camera modes
│   ├── combatsystem/     <- Damage resolution and hitbox detection
│   ├── editor/           <- In-game level editor (25 files)
│   ├── cutscene/         <- Cutscene triggers and camera control
│   ├── dialogue/         <- Dialogue boxes and registry
│   ├── save/             <- Save/load pipeline (DTO -> serializer -> repository)
│   ├── ui/               <- HUD, panels, iris transitions, UIScaler
│   └── infrastructure/   <- AssetManager, AudioManager, GifAnimation, AppPaths
├── third_party/
│   ├── raylib/           <- Precompiled Raylib (headers + .lib/.a for MSVC & MinGW)
│   └── json/             <- nlohmann/json (header-only, no compilation needed)
├── saves/                <- Runtime save data (gitignored, auto-created on first run)
├── CMakeLists.txt
├── CMakePresets.json     <- Build presets: windows-vs2022, windows-vs2019, windows-mingw
└── README.md
```

---

## Controls (Default — Fully Rebindable in Settings)

| Action | Player 1 | Player 2 |
|---|---|---|
| Move Left / Right | `A` / `D` | `←` / `→` |
| Jump | `W` | `↑` |
| Crouch / Drop-through | `S` | `↓` |
| Attack 1–4 (Combo chain) | `J` `K` `L` `;` | `Numpad 1` `2` `3` `4` |
| Jump Attack | `U` | `Numpad 7` |
| Low Attack | `I` | `Numpad 8` |
| Ranged / Special | `O` | `Numpad 9` |
| Dash | `H` | `Numpad 0` |
| Block | `N` | `Numpad 5` |
| Use Item | `M` | `Numpad 6` |

> All bindings are rebindable from **Main Menu → Settings** or from the **in-game settings panel** without leaving the level.

---

*Group 51 — Phạm Đức Minh (25125028) · Lê Tiến Bình (25125007) — CS202 · 25A01 · September 2026*
