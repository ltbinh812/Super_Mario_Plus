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
    include/ui/transitions
    include/core
    include/save
    include/entity
    include/entity/Player
    include/entity/Skill
    include/entity/Item
    include/entity/BuffEffects
    include/entity/EnemyStates
    include/entity/BossStates
    include/infrastructure
    include/command
    include/environment
    include/dialogue
    include/cutscene
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
    src/core/SettingsManager.cpp
    src/core/StateManager.cpp
    src/core/InputHandler.cpp
    src/core/SaveManager.cpp
    src/save/JsonSaveSerializer.cpp
    src/save/FileSaveRepository.cpp
    src/states/GameState.cpp
    src/editor/EditorBlockRegistry.cpp
    src/editor/AutoTiler.cpp
    src/editor/CustomMapSerializer.cpp
    src/editor/CustomMapValidator.cpp
    src/editor/PlaceTileTool.cpp
    src/editor/PlaceEntityTool.cpp
    src/editor/EraseTool.cpp
    src/editor/EditorToolManager.cpp
    src/editor/EditorCamera.cpp
    src/editor/EditorMapResizer.cpp
    src/editor/CategoryPanel.cpp
    src/editor/BlockVariantPanel.cpp
    src/editor/EditorBottomPanel.cpp
    src/editor/UndoRedoStack.cpp
    src/editor/EditorSaveLoadUI.cpp
    src/editor/EditorTextureCache.cpp
    src/editor/EntityPalette.cpp
    src/states/MapEditorState.cpp
    src/ui/Buttons.cpp
    src/ui/PlayerHUD.cpp
    src/ui/UIAtlasAnimator.cpp
    src/ui/CharacterInfoPanel.cpp
    src/ui/IngameSettingsPanel.cpp
    src/ui/ShopUIPanel.cpp
    src/ui/PanelButton.cpp
    src/ui/WorldActionPanel.cpp
    src/ui/SaveVersionPanel.cpp
    src/ui/UIScaler.cpp
    src/infrastructure/GifAnimation.cpp
    src/ui/transitions/IrisTransition.cpp
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
    src/entity/Skill/BasicMeleeEnemySkill.cpp
    src/entity/Skill/ProjectileEnemySkill.cpp
    src/entity/Skill/ExplosionEnemySkill.cpp
    src/entity/Fireball.cpp
    src/entity/Explosion.cpp
    src/entity/EntityFactory.cpp
    src/entity/Mob.cpp
    src/entity/Boss.cpp
    src/entity/EnemyFactory.cpp
    src/entity/BossStates/BossIdleState.cpp
    src/entity/BossStates/BossIntroState.cpp
    src/entity/BossStates/BossRunState.cpp
    src/entity/BossStates/BossAttackState.cpp
    src/entity/BossStates/BossPatrolState.cpp
    src/entity/BossStates/BossDieState.cpp
    src/entity/BossStates/BossHurtState.cpp
    src/entity/BossStates/BossSkillState.cpp
    src/entity/BossStates/BossDebugInputState.cpp
    src/entity/EnemyStates/EnemyIdleState.cpp
    src/entity/EnemyStates/EnemyPatrolState.cpp
    src/entity/EnemyStates/EnemyRunState.cpp
    src/entity/EnemyStates/EnemyAttackState.cpp
    src/entity/EnemyStates/EnemyHurtState.cpp
    src/entity/EnemyStates/EnemyDieState.cpp
    src/entity/EnemyStates/EnemySkillState.cpp
    src/entity/Item/ItemAtlasRegistry.cpp
    src/entity/Item/AtlasAnimation.cpp
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
    src/entity/Item/PoisonFlask.cpp
    src/entity/Item/ShopAsset.cpp
    src/entity/Item/EndgameAsset.cpp
    src/entity/Item/ItemFactory.cpp
    src/entity/Item/ItemUsageFactory.cpp
    src/combatsystem/BruteForceDetector.cpp
    src/combatsystem/CombatSystem.cpp
    src/infrastructure/AssetManager.cpp
    src/infrastructure/AudioManager.cpp
    src/environment/TileMap.cpp
    src/environment/MapCamera.cpp
    src/states/BaseLevelState.cpp
    src/states/WorldCatalog.cpp
    src/states/World01State.cpp
    src/states/World02State.cpp
    src/states/World03State.cpp
    src/states/World04State.cpp
    src/states/World05State.cpp
    src/states/World06State.cpp
    src/states/MainMenuState.cpp
    src/states/MapSelectionState.cpp
    src/states/CharacterSelectionState.cpp
    src/states/LoadingState.cpp
    src/states/EndgameState.cpp
    src/ui/ButtonGroup.cpp
    src/environment/CameraFollowMode.cpp
    src/environment/CameraPanMode.cpp
    src/environment/CameraZoomMode.cpp
    src/dialogue/DialogueLoader.cpp
    src/dialogue/DialogueBox.cpp
    src/dialogue/DialogueRegistry.cpp
    src/cutscene/CutsceneManager.cpp
    src/cutscene/CutsceneTrigger.cpp
)

# Executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link raylib
target_link_libraries(${PROJECT_NAME} ${RAYLIB_LIB} ${PLATFORM_LIBS})

# -----------------------------------------------------------------------------
# Loại bỏ section không dùng khi liên kết (GCC/MinGW).
#
# BẮT BUỘC, không phải tối ưu cho vui: dự án đã chạm trần tầm với của
# relocation 32-bit trên Windows x86-64. Thiếu cờ này, ld dừng với:
#
#   relocation truncated to fit: IMAGE_REL_AMD64_REL32 against `.text$_ZN8nlohmann...'
#
# Nguyên nhân là nlohmann/json sinh ra rất nhiều hàm template với tên cực dài;
# mỗi .cpp có #include <nlohmann/json.hpp> lại tạo một bản COMDAT riêng, khiến
# ảnh chạy phình quá ±2GB mà lệnh nhảy 32-bit với tới được.
#
# -ffunction-sections/-fdata-sections đặt mỗi hàm và mỗi biến vào section
# riêng, --gc-sections vứt bỏ những section không ai gọi tới. Bản Debug từ
# ~180MB object rút còn ~100MB exe và liên kết trót lọt.
# -----------------------------------------------------------------------------
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE -ffunction-sections -fdata-sections)
    target_link_options(${PROJECT_NAME} PRIVATE -Wl,--gc-sections)
endif()

# Add Include Directories
target_include_directories(${PROJECT_NAME} PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include 
    ${CMAKE_CURRENT_SOURCE_DIR}/include/core
    ${CMAKE_CURRENT_SOURCE_DIR}/include/editor
    ${CMAKE_CURRENT_SOURCE_DIR}/include/states
)

# Copy assets folder to build directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
)