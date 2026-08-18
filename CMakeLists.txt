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
    include/entity/Item
    include/entity/BuffEffects
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
    src/core/SaveManager.cpp
    src/states/GameState.cpp
    src/ui/Buttons.cpp
    src/ui/PlayerHUD.cpp
    src/entity/Animation.cpp
    src/entity/Entity.cpp
    src/entity/Effects.cpp
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
    src/entity/Player/BuffManager.cpp
    src/entity/Skill/DashSkill.cpp
    src/entity/Skill/Attack1Skill.cpp
    src/entity/Skill/Attack2Skill.cpp
    src/entity/Skill/Attack3Skill.cpp
    src/entity/Skill/Attack4Skill.cpp
    src/entity/Skill/BlockSkill.cpp
    src/entity/Skill/LongAttackSkill.cpp
    src/entity/Skill/JumpAttackSkill.cpp
    src/entity/Skill/LowAttackSkill.cpp
    src/entity/Skill/SpecialSkillAttack.cpp
    src/entity/Fireball.cpp
    src/entity/Explosion.cpp
    src/entity/EntityFactory.cpp
    src/entity/Item/ItemAtlasRegistry.cpp
    src/entity/Item/BaseItem.cpp
    src/entity/Item/Spring.cpp
    src/entity/Item/Coin.cpp
    src/entity/Item/Key.cpp
    src/entity/Item/Door.cpp
    src/entity/Item/Flag.cpp
    src/entity/Item/LuckyBlock.cpp
    src/entity/Item/ChestNormal.cpp
    src/entity/Item/ChestBoss.cpp
    src/entity/Item/Buff.cpp
    src/entity/Item/Boom.cpp
    src/entity/Item/ItemFactory.cpp
    src/entity/Item/ItemUsageFactory.cpp
    src/combatsystem/BruteForceDetector.cpp
    src/combatsystem/CombatSystem.cpp
    src/infrastructure/AssetManager.cpp
    src/environment/TileMap.cpp
    src/environment/MapCamera.cpp
    src/states/BaseLevelState.cpp
    src/states/World01State.cpp
    src/states/World02State.cpp
    src/states/World03State.cpp
    src/states/World04State.cpp
    src/states/World05State.cpp
    src/states/World06State.cpp
    src/states/CharacterSelectionState.cpp
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