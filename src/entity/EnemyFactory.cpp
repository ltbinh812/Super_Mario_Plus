#include "EnemyFactory.h"
#include "Mob.h"
#include "Item/AtlasAnimation.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <raylib.h>

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
    
    auto& attackData = mobData["attackData"];
    config.attackData.damage = attackData["damage"].get<int>();
    config.attackData.hitboxStartFrame = attackData["hitboxStartFrame"].get<int>();
    config.attackData.hitboxEndFrame = attackData["hitboxEndFrame"].get<int>();
    config.attackData.hitboxTotalFrames = attackData["hitboxTotalFrames"].get<int>();
    config.attackData.frameTime = attackData["frameTime"].get<float>();
    config.attackData.box = { 
        attackData["box"]["offsetX"].get<float>(),
        attackData["box"]["offsetY"].get<float>(),
        attackData["box"]["w"].get<float>(),
        attackData["box"]["h"].get<float>()
    };

    auto& aiData = mobData["aiData"];
    config.aiData.detectionRange = aiData["detectionRange"].get<float>();
    config.aiData.attackRange = aiData["attackRange"].get<float>();
    config.aiData.patrolSpeed = aiData["patrolSpeed"].get<float>();
    config.aiData.patrolTime = aiData["patrolTime"].get<float>();

    auto mob = std::make_unique<BasicMob>(worldPos, lowerKey, bStats, config);
    
    auto& anims = mobData["animationFrames"];
    mob->initAnimations(
        anims["attack"].get<int>(),
        anims["run"].get<int>(),
        anims["idle"].get<int>(),
        anims["hurt"].get<int>(),
        anims["die"].get<int>()
    );
    
    // Start in Patrol state
    mob->changeState(std::make_unique<EnemyPatrolState>());
    
    TraceLog(LOG_INFO, "[EnemyFactory] Successfully spawned %s", key.c_str());
    return mob;
}
