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
    BasicMob(Vector2 worldPos, const std::string& type, const CharacterBaseStats& bStats)
        : Mob(worldPos, type, bStats) {}
        
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

    // Defaults
    CharacterBaseStats bStats;
    bStats.maxHealth = 50;
    bStats.moveVelocity = 100.0f;
    bStats.jumpVelocity = 300.0f;
    bStats.gravityScale = 10.0f;
    bStats.physicsBox = { 32.0f, 32.0f };

    // We can load from enemies.json later, for now hardcode Mob_mushroom
    if (identifier == "Mob_mushroom" || identifier == "Mob_Test") {
        auto mob = std::make_unique<BasicMob>(worldPos, "mob_mushroom", bStats);
        
        // attack(10), run(8), idle(7), hurt(5), die(15)
        mob->initAnimations(10, 8, 7, 5, 15);
        
        // Start in Patrol state
        mob->changeState(std::make_unique<EnemyPatrolState>());
        
        TraceLog(LOG_INFO, "[EnemyFactory] Successfully spawned %s", identifier.c_str());
        return mob;
    }

    TraceLog(LOG_INFO, "[EnemyFactory] Unknown identifier: %s", identifier.c_str());
    return nullptr;
}
