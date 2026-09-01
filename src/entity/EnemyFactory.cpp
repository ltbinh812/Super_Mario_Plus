#include "EnemyFactory.h"
#include "Mob.h"
#include "Boss.h"
#include "Item/AtlasAnimation.h"
#include "infrastructure/AssetManager.h"
#include "Skill/BasicMeleeEnemySkill.h"
#include "Skill/ProjectileEnemySkill.h"
#include "Skill/ExplosionEnemySkill.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <raylib.h>
#include <filesystem>

// Include states here once implemented
#include "EnemyStates/EnemyIdleState.h"
#include "EnemyStates/EnemyPatrolState.h"

using json = nlohmann::json;

class BasicMob : public Mob {
public:
    BasicMob(Vector2 worldPos, const std::string& type, const CharacterBaseStats& bStats, const MobConfig& config)
        : Mob(worldPos, type, bStats, config) {}
        
    void initAnimations(int attackFrames, int runFrames, int idleFrames, int hurtFrames, int dieFrames) {
        // Assuming sprite JSON is loaded in ItemAtlasRegistry
        std::string lowerType = mobType;
        std::transform(lowerType.begin(), lowerType.end(), lowerType.begin(), ::tolower);
        
        animations[lowerType + "_attack"] = AtlasAnimation(lowerType + "_attack", attackFrames, 0.1f, true);
        animations[lowerType + "_run"] = AtlasAnimation(lowerType + "_run", runFrames, 0.1f, true);
        animations[lowerType + "_idle"] = AtlasAnimation(lowerType + "_idle", idleFrames, 0.1f, true);
        animations[lowerType + "_hurt"] = AtlasAnimation(lowerType + "_hurt", hurtFrames, 0.1f, false);
        animations[lowerType + "_die"] = AtlasAnimation(lowerType + "_die", dieFrames, 0.1f, false);
        
        setAnimation(lowerType + "_idle");
    }
};

std::unique_ptr<Entity> EnemyFactory::create(
    const std::string& identifier,
    Vector2 worldPos,
    const json& fieldInstances)
{
    TraceLog(LOG_INFO, "[EnemyFactory] Spawn requested for: %s at (%f, %f)", identifier.c_str(), worldPos.x, worldPos.y);

    std::ifstream file("assets/config/enemies.json");
    if (!file.is_open()) {
        TraceLog(LOG_ERROR, "[EnemyFactory] Cannot open assets/config/enemies.json");
        return nullptr;
    }

    json jsonData;
    file >> jsonData;

    // Default identifier if not found, or maybe fallback. But we use Mob_mushroom for testing often.
    std::string key = identifier;
    if (key == "Mob_Test") key = "Mob_mushroom"; // fallback for old maps
    
    // Convert to lowercase to match json keys since it might be "mob_mushroom"
    std::string lowerKey = key;
    std::transform(lowerKey.begin(), lowerKey.end(), lowerKey.begin(), ::tolower);

    if (!jsonData.contains(lowerKey)) {
        TraceLog(LOG_ERROR, "[EnemyFactory] Unknown identifier in json: %s", lowerKey.c_str());
        return nullptr;
    }

    auto& mobData = jsonData[lowerKey];

    CharacterBaseStats bStats;
    bStats.maxHealth = mobData["maxHealth"].get<int>();
    bStats.moveVelocity = mobData["moveVelocity"].get<float>();
    bStats.jumpVelocity = mobData["jumpVelocity"].get<float>();
    bStats.gravityScale = mobData["gravityScale"].get<float>();
    bStats.physicsBox = { mobData["physicsBox"]["w"].get<float>(), mobData["physicsBox"]["h"].get<float>() };

    MobConfig config;
    config.name = lowerKey;
    
    if (mobData.contains("attackData")) {
        auto& attackData = mobData["attackData"];
        config.attackData.damage = attackData.value("damage", 10);
        config.attackData.hitboxStartFrame = attackData.value("hitboxStartFrame", 0);
        config.attackData.hitboxEndFrame = attackData.value("hitboxEndFrame", 0);
        config.attackData.hitboxTotalFrames = attackData.value("hitboxTotalFrames", 1);
        config.attackData.frameTime = attackData.value("frameTime", 0.1f);
        if (attackData.contains("box")) {
            config.attackData.box = { 
                attackData["box"].value("offsetX", 0.0f),
                attackData["box"].value("offsetY", 0.0f),
                attackData["box"].value("w", 0.0f),
                attackData["box"].value("h", 0.0f)
            };
        }
    } else {
        config.attackData = {};
    }

    auto& aiData = mobData["aiData"];
    config.aiData.detectionRange = aiData["detectionRange"].get<float>();
    config.aiData.attackRange = aiData["attackRange"].get<float>();
    config.aiData.patrolSpeed = aiData["patrolSpeed"].get<float>();
    config.aiData.patrolTime = aiData["patrolTime"].get<float>();

    std::unique_ptr<Mob> mob = nullptr;

    if (key.find("Boss_") == 0) {
        // Parse cutsceneId from LDtk fieldInstances
        std::string cutsceneId = "";
        if (fieldInstances.is_array()) {
            for (auto& field : fieldInstances) {
                if (field["__identifier"] == "cutsceneId" && field.contains("__value") && !field["__value"].is_null()) {
                    cutsceneId = field["__value"].get<std::string>();
                }
            }
        }
        mob = std::make_unique<Boss>(worldPos, lowerKey, bStats, config, cutsceneId);
        // State will be set by Boss constructor based on cutsceneId
    } else {
        mob = std::make_unique<BasicMob>(worldPos, lowerKey, bStats, config);
        // Start in Patrol state for normal mobs
        mob->changeState(std::make_unique<EnemyPatrolState>());
    }

    // NEW: Check for Standard Animations vs Atlas Animations
    std::string assetFolder = "";
    if (mobData.contains("assetFolder")) {
        assetFolder = mobData["assetFolder"].get<std::string>();
    }

    if (mobData.contains("animations")) {
        std::unordered_map<std::string, Animation> standardAnims;
        
        for (auto& [animName, animData] : mobData["animations"].items()) {
            std::string texBase = animData["texture"].get<std::string>();
            std::string texKey  = lowerKey + "_" + texBase;
            std::string texPath = "assets/" + assetFolder + "/" + texBase + ".png";
            
            AssetManager::getInstance().loadTexture(texKey, texPath);
            
            if (!assetFolder.empty()) {
                std::string soundPath = "assets/" + assetFolder + "/sounds/" + animName + ".wav";
                std::string soundKey  = lowerKey + "_" + animName + "_sound";
                if (std::filesystem::exists(soundPath)) {
                    AssetManager::getInstance().loadSound(soundKey, soundPath);
                }
            }
            
            float scale = animData.value("scale", 1.0f);
            Animation anim(
                AssetManager::getInstance().getTexture(texKey),
                animData["frameNum"].get<int>(),
                animData["frameTime"].get<float>(),
                scale
            );
            
            if (animData.contains("loop")) {
                anim.setLoop(animData["loop"].get<bool>());
            } else {
                anim.setLoop(animName == "idle" || animName == "patrol" || animName == "run");
            }
            
            standardAnims.emplace(animName, anim);
        }
        mob->setStandardAnimations(std::move(standardAnims));
        mob->setAnimation("idle");
    } else if (mobData.contains("animationFrames")) {
        // FALLBACK: Atlas Animation
        auto& anims = mobData["animationFrames"];
        if (auto* boss = dynamic_cast<Boss*>(mob.get())) {
            int introFrames = anims.contains("intro") ? anims["intro"].get<int>() : 0;
            boss->initAnimations(
                anims["attack"].get<int>(),
                anims["run"].get<int>(),
                anims["idle"].get<int>(),
                anims["hurt"].get<int>(),
                anims["die"].get<int>(),
                introFrames
            );
        } else if (auto* basicMob = dynamic_cast<BasicMob*>(mob.get())) {
            basicMob->initAnimations(
                anims["attack"].get<int>(),
                anims["run"].get<int>(),
                anims["idle"].get<int>(),
                anims["hurt"].get<int>(),
                anims["die"].get<int>()
            );
        }
        
        // Load sounds for Atlas Animations
        if (!assetFolder.empty()) {
            std::vector<std::string> baseAnims = {"attack", "run", "idle", "hurt", "die", "intro", "patrol"};
            for (const std::string& animName : baseAnims) {
                if (anims.contains(animName) || animName == "intro" || animName == "patrol") {
                    std::string soundPath = "assets/" + assetFolder + "/sounds/" + animName + ".wav";
                    std::string soundKey = lowerKey + "_" + animName + "_sound";
                    if (std::filesystem::exists(soundPath)) {
                        AssetManager::getInstance().loadSound(soundKey, soundPath);
                    }
                }
            }
        }
    }

    // NEW: Parse soundFrames for Animation Events
    if (mobData.contains("soundFrames") && !assetFolder.empty()) {
        std::unordered_map<std::string, std::unordered_map<int, std::string>> soundFrames;
        for (auto& [animName, frameData] : mobData["soundFrames"].items()) {
            for (auto& [frameIdxStr, soundSuffix] : frameData.items()) {
                int frameIdx = std::stoi(frameIdxStr);
                std::string suffix = soundSuffix.get<std::string>();
                
                soundFrames[animName][frameIdx] = suffix;
                
                // Load the sound file into AssetManager
                std::string soundPath = "assets/" + assetFolder + "/sounds/" + suffix + ".wav";
                // The soundKey for frame-based sounds will be specific to the mob and suffix
                std::string soundKey = lowerKey + "_" + suffix + "_sound";
                std::cout << "[DEBUG-FACTORY] Attempting to load mob sound: " << soundKey << " from " << soundPath << std::endl;
                if (std::filesystem::exists(soundPath)) {
                    std::cout << "[DEBUG-FACTORY] Mob Path exists! Loading..." << std::endl;
                    AssetManager::getInstance().loadSound(soundKey, soundPath);
                } else {
                    std::cout << "[ERROR-FACTORY] Mob Path does NOT exist: " << soundPath << std::endl;
                }
            }
        }
        mob->setSoundFrames(std::move(soundFrames));
    }

    // NEW: Check for Skills
    if (mobData.contains("enemySkills")) {
        for (auto& [skillName, skillData] : mobData["enemySkills"].items()) {
            // For now, map all to BasicMeleeEnemySkill, in the future we can use a Registry
            int dmg = skillData.value("damage", 10);
            
            // Calculate hitboxes based on frameTime if available
            float frameTime = 0.1f;
            int totalFrames = 1;
            
            if (mobData.contains("animations") && mobData["animations"].contains(skillName)) {
                frameTime = mobData["animations"][skillName].value("frameTime", 0.1f);
                totalFrames = mobData["animations"][skillName].value("frameNum", 1);
            } else if (mobData.contains("animationFrames")) {
                // If using atlas, skillName usually maps to something like "mob_mushroom_attack"
                // but the key in animationFrames is just "attack".
                // We'll just extract the suffix after the last underscore if present.
                std::string baseName = skillName;
                size_t lastUnderscore = skillName.find_last_of('_');
                if (lastUnderscore != std::string::npos) {
                    baseName = skillName.substr(lastUnderscore + 1);
                }
                if (mobData["animationFrames"].contains(baseName)) {
                    totalFrames = mobData["animationFrames"][baseName].get<int>();
                }
            }
            
            int startFrame = skillData.value("hitboxStartFrame", 0);
            int endFrame = skillData.value("hitboxEndFrame", 1);
            
            float startTime = startFrame * frameTime;
            float endTime = endFrame * frameTime;
            float duration = totalFrames * frameTime;

            Rectangle box = {0,0,0,0};
            if (skillData.contains("box")) {
                box.x = skillData["box"].value("offsetX", 0.0f);
                box.y = skillData["box"].value("offsetY", 0.0f);
                box.width = skillData["box"].value("w", 0.0f);
                box.height = skillData["box"].value("h", 0.0f);
            }

            std::unique_ptr<IEnemySkill> createdSkill = nullptr;

            if (skillName.find("explosion") != std::string::npos) {
                createdSkill = std::make_unique<ExplosionEnemySkill>(
                    skillName, dmg, startTime, endTime, duration
                );
            } else if (skillName.find("special") != std::string::npos || skillName.find("special_ball") != std::string::npos) {
                createdSkill = std::make_unique<ProjectileEnemySkill>(
                    skillName, dmg, startTime, endTime, duration, EntityType::SpecialBall
                );
            } else if (skillName.find("long") != std::string::npos || skillName.find("fireball") != std::string::npos || skillName.find("projectile") != std::string::npos) {
                createdSkill = std::make_unique<ProjectileEnemySkill>(
                    skillName, dmg, startTime, endTime, duration, EntityType::Fireball
                );
            } else {
                createdSkill = std::make_unique<BasicMeleeEnemySkill>(
                    skillName, dmg, startTime, endTime, duration, box
                );
            }

            // Set configuration
            createdSkill->setDashMultiplier(skillData.value("dashMultiplier", 2.0f));

            mob->addEnemySkill(std::move(createdSkill));
        }
    }
    
    TraceLog(LOG_INFO, "[EnemyFactory] Successfully spawned %s", key.c_str());
    return mob;
}
