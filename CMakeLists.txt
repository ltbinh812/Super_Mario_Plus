cmake_minimum_required(VERSION 3.11)
project(SuperMarioPlus)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Configure raylib paths
set(RAYLIB_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/third_party/raylib/include")

if(WIN32)
    if(MINGW)
        set(RAYLIB_LIB "${CMAKE_SOURCE_DIR}/third_party/raylib/lib/windows_mingw/libraylib.a")
    elseif(MSVC)
        set(RAYLIB_LIB "${CMAKE_SOURCE_DIR}/third_party/raylib/lib/windows_msvc/raylib.lib")
    endif()
    set(PLATFORM_LIBS winmm gdi32)
endif()

# Include directories
include_directories(${RAYLIB_INCLUDE_DIR} include)

# Source files
set(SOURCES 
    src/main.cpp
    src/Game.cpp
    src/IntroState.cpp
    src/SettingState.cpp
    src/StateManager.cpp
    src/UIComponent.cpp

)

# Executable
add_executable(${PROJECT_NAME} ${SOURCES}
    )

# Link raylib
target_link_libraries(${PROJECT_NAME} ${RAYLIB_LIB} ${PLATFORM_LIBS})
