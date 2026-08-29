#include "BossStates/BossDebugInputState.h"
#include "BossStates/BossSkillState.h"
#include "Mob.h"
#include "Skill/IEnemySkill.h"
#include <raylib.h>
#include <iostream>
#include <string>

void BossDebugInputState::enter(Mob& mob) {
    mob.setVelocity({0.0f, mob.getVelocity().y});
    mob.setAnimation("idle");
    // std::cout << "[BossDebugInputState] Entered manual testing mode.\n";
}

void BossDebugInputState::decideAction(Mob& mob) {
    // Check for skill triggers based on key presses
    std::string skillToTrigger = "";

    if (IsKeyPressed(KEY_ONE)) skillToTrigger = "attack_1";
    else if (IsKeyPressed(KEY_TWO)) skillToTrigger = "attack_2";
    else if (IsKeyPressed(KEY_THREE)) skillToTrigger = "attack_3";
    else if (IsKeyPressed(KEY_FOUR)) skillToTrigger = "attack_4";
    else if (IsKeyPressed(KEY_FIVE)) skillToTrigger = "special_attack";
    else if (IsKeyPressed(KEY_SIX)) skillToTrigger = "explosion_attack";
    else if (IsKeyPressed(KEY_SEVEN)) skillToTrigger = "long_attack";

    if (!skillToTrigger.empty()) {
        // Find the skill
        IEnemySkill* targetSkill = nullptr;
        for (const auto& skill : mob.getEnemySkills()) {
            if (skill->getAnimName() == skillToTrigger) {
                targetSkill = skill.get();
                break;
            }
        }
        
        if (targetSkill) {
            // std::cout << "[BossDebugInputState] Triggering skill: " << skillToTrigger << "\n";
            auto state = std::make_unique<BossSkillState>(targetSkill);
            // We want it to come back to BossDebugInputState after skill finishes
            state->setReturnToDebugMode(true);
            mob.changeState(std::move(state));
            return;
        } else {
            // std::cout << "[BossDebugInputState] Skill not found on boss: " << skillToTrigger << "\n";
        }
    }
}

void BossDebugInputState::process(Mob& mob) {
    float moveSpeed = mob.getConfig().aiData.patrolSpeed * 1.5f;
    
    if (IsKeyDown(KEY_LEFT)) {
        mob.setVelocity({-moveSpeed, mob.getVelocity().y});
        mob.setFacingRight(false);
        mob.setAnimation("run");
    } else if (IsKeyDown(KEY_RIGHT)) {
        mob.setVelocity({moveSpeed, mob.getVelocity().y});
        mob.setFacingRight(true);
        mob.setAnimation("run");
    } else {
        mob.setVelocity({0.0f, mob.getVelocity().y});
        mob.setAnimation("idle");
    }
    // Debug log
    static float timer = 0;
    timer += GetFrameTime();
    if (timer > 0.5f) {
        // TraceLog(LOG_INFO, "[BossDebug] Speed: %f, VelX: %f", moveSpeed, mob.getVelocity().x);
        timer = 0.0f;
    }
}

void BossDebugInputState::exit(Mob& mob) {
    // std::cout << "[BossDebugInputState] Exiting manual testing mode.\n";
}
