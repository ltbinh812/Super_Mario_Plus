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
    include/states
    include/ui
    include/core
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
    src/core/Game.cpp
    src/core/StateManager.cpp
    src/states/GameState.cpp
    src/states/IntroState.cpp
    src/states/SettingState.cpp
    src/states/Menu.cpp
    src/states/World.cpp
    src/ui/SettingsOverlay.cpp
    src/ui/UIComponent.cpp
)

# Executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link raylib
target_link_libraries(${PROJECT_NAME} ${RAYLIB_LIB} ${PLATFORM_LIBS})
