#include "Boss.h"
#include "BossStates/BossIdleState.h"
#include "EnemyStates/EnemyRunState.h"
#include "BossStates/BossIntroState.h"
#include "BossStates/BossHurtState.h"
#include "BossStates/BossDieState.h"
#include <iostream>

Boss::Boss(Vector2 worldPos, const std::string& mobType, const CharacterBaseStats& bStats, const MobConfig& config, const std::string& cutsceneId)
    : Mob(worldPos, mobType, bStats, config), cutsceneId(cutsceneId) {
    
    isWaitingForCutscene = !this->cutsceneId.empty();
    
    // Fail-safe initialization based on cutsceneId
    if (this->cutsceneId.empty()) {
        // std::cout << "[Boss] No cutsceneId provided for Boss. Skipping Intro and defaulting to Idle State.\n";
        changeState(std::make_unique<BossIdleState>());
    } else {
        // std::cout << "[Boss] Initialized with cutsceneId: " << this->cutsceneId << ". Waiting for Intro.\n";
        changeState(std::make_unique<BossIdleState>());
    }
}

#include "BossStates/BossDebugInputState.h"
#include <raylib.h>

void Boss::update(float dt) {
    Mob::update(dt);
    
    static float t = 0.0f;
    t += dt;
    if (t > 1.0f) {
        // TraceLog(LOG_INFO, "[Boss] update() is running.");
        t = 0.0f;
    }
    
    // Toggle Debug Mode with F1
    if (IsKeyPressed(KEY_P)) {
        isDebugMode = !isDebugMode;
        if (isDebugMode) {
            // std::cout << "[Boss] Debug Mode ENABLED. Entering BossDebugInputState.\n";
            changeState(std::make_unique<BossDebugInputState>());
        } else {
            // std::cout << "[Boss] Debug Mode DISABLED. Reverting to Idle State.\n";
            changeState(std::make_unique<BossIdleState>());
        }
    }
}

void Boss::onCutsceneStart(const std::string& triggerId) {
    if (!cutsceneId.empty() && cutsceneId == triggerId) {
        // std::cout << "[Boss] Cutscene started matching my ID (" << cutsceneId << "). Entering Intro State.\n";
        isDebugMode = false; // Tắt debug nếu cutscene bắt đầu đè lên
        isWaitingForCutscene = false;
        changeState(std::make_unique<BossIntroState>());
    }
}

void Boss::onCutsceneEnd(const std::string& triggerId) {
    if (!cutsceneId.empty() && cutsceneId == triggerId) {
        // std::cout << "[Boss] Cutscene ended matching my ID (" << cutsceneId << "). Notifying Intro State.\n";
        
        // Notify the current state (if it's BossIntroState) that cutscene is finished
        // We can do this by using dynamic_cast
        if (auto* introState = dynamic_cast<BossIntroState*>(currentState.get())) {
            introState->markCutsceneFinished();
        }
    }
}

void Boss::initAnimations(int attackFrames, int runFrames, int idleFrames, int hurtFrames, int dieFrames, int introFrames) {
    std::string lowerType = mobType;
    std::transform(lowerType.begin(), lowerType.end(), lowerType.begin(), ::tolower);
    
    animations[lowerType + "_attack"] = AtlasAnimation(lowerType + "_attack", attackFrames, 0.1f, true);
    animations[lowerType + "_run"] = AtlasAnimation(lowerType + "_run", runFrames, 0.1f, true);
    animations[lowerType + "_idle"] = AtlasAnimation(lowerType + "_idle", idleFrames, 0.1f, true);
    animations[lowerType + "_hurt"] = AtlasAnimation(lowerType + "_hurt", hurtFrames, 0.1f, false);
    animations[lowerType + "_die"] = AtlasAnimation(lowerType + "_die", dieFrames, 0.1f, false);
    
    if (introFrames > 0) {
        animations[lowerType + "_intro"] = AtlasAnimation(lowerType + "_intro", introFrames, 0.1f, false);
    }
    
    setAnimation(lowerType + "_idle");
}

void Boss::takeDamage(int damage, float knockbackDirX, bool forceInterrupt) {
    if (isDead || hurtTimer > 0.0f) return;

    runtimeStats.health -= damage;

    if (runtimeStats.health <= 0) {
        runtimeStats.health = 0;
        isDead = true;
        changeState(std::make_unique<BossDieState>());
    } else if (forceInterrupt) {
        changeState(std::make_unique<BossHurtState>());
    }
}

