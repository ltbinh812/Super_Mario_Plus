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
    third_party/json/include
    include
    include/combatsystem
    include/states
    include/ui
    include/core
    include/entity
    include/entity/Player
    include/entity/Skill
    include/infrastructure
    include/command
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
    src/core/Game.cpp
    src/core/StateManager.cpp
    src/core/InputHandler.cpp
    src/states/GameState.cpp
    src/ui/Buttons.cpp
    src/entity/Animation.cpp
    src/entity/Entity.cpp
    src/entity/Player/Player.cpp
    src/entity/Player/PlayerState.cpp
    src/entity/Player/PlayerIdleState.cpp
    src/entity/Player/PlayerRunState.cpp
    src/entity/Player/PlayerJumpState.cpp
    src/entity/Player/PlayerFallState.cpp
    src/entity/Player/PlayerCrouchState.cpp
    src/entity/Player/PlayerHurtState.cpp
    src/entity/Player/PlayerDieState.cpp
    src/entity/Player/PlayerSkillState.cpp
    src/entity/Player/PlayerSwimState.cpp
    src/entity/Player/PlayerClimbState.cpp
    src/entity/Player/PlayerFactory.cpp
    src/entity/Skill/DashSkill.cpp
    src/entity/Skill/Attack1Skill.cpp
    src/entity/Skill/Attack2Skill.cpp
    src/entity/Skill/Attack3Skill.cpp
    src/entity/Skill/Attack4Skill.cpp
    src/entity/Skill/BlockSkill.cpp
    src/entity/Skill/LongAttackSkill.cpp
    src/entity/Skill/JumpAttackSkill.cpp
    src/entity/Skill/LowAttackSkill.cpp
    src/entity/Fireball.cpp
    src/entity/Explosion.cpp
    src/entity/EntityFactory.cpp
    src/combatsystem/BruteForceDetector.cpp
    src/combatsystem/CombatSystem.cpp
    src/infrastructure/AssetManager.cpp
    src/environment/TileMap.cpp
    src/environment/MapCamera.cpp
    src/states/World01State.cpp
    src/states/World02State.cpp
    src/states/World03State.cpp
)

# Executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link raylib
target_link_libraries(${PROJECT_NAME} ${RAYLIB_LIB} ${PLATFORM_LIBS})

# Copy assets folder to build directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
)