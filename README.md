# Super Mario Plus

A lightweight 2D platformer project built with C++ and [Raylib](https://www.raylib.com/).

This project is completely self-contained and pre-configured to be lightweight (under 20MB). It uses precompiled Raylib static libraries, meaning you **do not** need a package manager like `vcpkg`, and you don't need to compile the Raylib source code yourself. 

## Requirements
- CMake (3.19 or higher is recommended for Presets support)
- A C++ Compiler:
  - **Windows**: Visual Studio (MSVC) **OR** MinGW-w64 (GCC)

## How to Build and Run

You can build the project effortlessly using the provided `CMakePresets.json`.

### Option 1: Using Visual Studio Code (Recommended)
1. Open this project folder in VS Code.
2. Install the **CMake Tools** and **C/C++** extensions.
3. VS Code will automatically detect the `CMakePresets.json` file. It will ask you to select a Configure Preset. Select either **Windows (MSVC)** or **Windows (MinGW)** depending on your installed compiler.
4. Click the **Build** button (or press `F7`).
5. Click the **Play / Launch** button (or press `Shift + F5`) to run the game!

### Option 2: Using the Terminal (Command Line)

**If you are using Visual Studio (MSVC):**
```bash
# 1. Configure the project
cmake --preset windows-msvc

# 2. Build the project
cmake --build --preset windows-msvc

# 3. Run the executable
.\build\msvc\Debug\SuperMarioPlus.exe
```

**If you are using MinGW / GCC:**
```bash
# 1. Configure the project
cmake --preset windows-mingw

# 2. Build the project
cmake --build --preset windows-mingw

# 3. Run the executable
.\build\mingw\SuperMarioPlus.exe
```

## Project Structure
- `src/`: Contains your C++ source code.
- `third_party/raylib/`: Contains the precompiled Raylib headers (`include`) and static libraries (`lib`). 
- `CMakeLists.txt`: The CMake build configuration file.
- `CMakePresets.json`: Pre-configured environment settings to make building easy without typing long terminal commands.
