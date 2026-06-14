cmake_minimum_required(VERSION 3.11)
project(SuperMarioPlus)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Configure raylib paths
set(RAYLIB_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/third_party/raylib/include")

# Add header files
include_directories(
    ${RAYLIB_INCLUDE_DIR}
    include
    include/characters
    include/abilities
    include/states
    include/ui
    include/core
    include/environment
)

if(WIN32)
    if(MINGW)
        set(RAYLIB_LIB "${CMAKE_SOURCE_DIR}/third_party/raylib/lib/windows_mingw/libraylib.a")
    elseif(MSVC)
        set(RAYLIB_LIB "${CMAKE_SOURCE_DIR}/third_party/raylib/lib/windows_msvc/raylib.lib")
    endif()
    set(PLATFORM_LIBS winmm gdi32)
endif()

# Source files
set(SOURCES 
    src/core/main.cpp
    src/core/GameManager.cpp
    src/core/CameraManager.cpp
    src/states/Menu.cpp
    src/states/World.cpp
    src/states/PlayerStates.cpp
    src/characters/Character.cpp
    src/characters/Player.cpp
    src/characters/CharacterFactory.cpp
    src/characters/Mario.cpp
    src/characters/Luigi.cpp
    src/characters/Peach.cpp
    src/characters/Toad.cpp
    src/characters/Wario.cpp
    src/characters/Goomba.cpp
    src/ui/HUD.cpp
    src/ui/SettingsOverlay.cpp
    src/ui/CharacterSelectionOverlay.cpp
    src/environment/Platform.cpp
    src/environment/Cloud.cpp
    src/environment/Decoration.cpp
    src/environment/Background.cpp
    src/core/SuperMarioPlus.cpp
)

# Executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link raylib
target_link_libraries(${PROJECT_NAME} ${RAYLIB_LIB} ${PLATFORM_LIBS})
