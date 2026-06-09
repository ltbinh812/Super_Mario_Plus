# SuperMarioPlus Project Structure & Progress

**Project Purpose:** A 2D platformer game (Super Mario style) built using C++ and Raylib.
**Current Progress:** Refactored project to strict OOP (One Class Per File) and implemented the **State Pattern**. Game entities (`Player`, `Platform`, `Cloud`, `Decoration`, `CameraManager`, `Background`, `HUD`) are independent classes. The `main.cpp` only initializes the window, and `SuperMarioPlus.cpp` runs a clean state machine loop via `GameManager`.

## File & Class Relationships

```text
SuperMarioPlus/
├── src/
│   ├── main.cpp
│   ├── SuperMarioPlus.cpp
│   ├── GameManager.cpp
│   ├── Menu.cpp
│   ├── World.cpp
│   ├── Player.cpp
│   ├── Platform.cpp
│   ├── Cloud.cpp
│   ├── Decoration.cpp
│   ├── CameraManager.cpp
│   ├── Background.cpp
│   └── HUD.cpp
└── include/
    ├── GameState.h
    ├── GameManager.h
    ├── Menu.h
    ├── World.h
    ├── SuperMarioPlus.h
    ├── Player.h
    ├── Platform.h
    ├── Cloud.h
    ├── Decoration.h
    ├── CameraManager.h
    ├── Background.h
    └── HUD.h
```

### Detailed File / Class Information

#### `main.cpp`
- **Purpose**: Entry point. Opens window and calls `RunGame()`.

#### `SuperMarioPlus.h` / `SuperMarioPlus.cpp`
- **Purpose**: Exposes `RunGame()`. Runs the pure State Pattern loop (`HandleInput()`, `Update()`, `Draw()`) via `GameManager`.

#### `GameState.h` & `GameManager.h/cpp`
- **Purpose**: State Pattern core. `GameState` is an abstract interface. `GameManager` holds `std::unique_ptr<GameState>` and delegates actions.

#### `Menu.h` / `Menu.cpp`
- **Purpose**: Implements `MenuState` (inherits `GameState`). Displays title and handles "Press ENTER to Start" transition to `World1_1State`.

#### `World.h` / `World.cpp`
- **Purpose**: Implements `World1_1State` (inherits `GameState`). Manages all gameplay objects (Player, Platforms, Clouds, etc.).

*(Other game entities like Player, Platform, Cloud, CameraManager, Background, HUD remain encapsulated as independent renderer and logic classes).*

---
*Note: History of monolithic structure has been pruned to keep documentation concise as per rules.*
