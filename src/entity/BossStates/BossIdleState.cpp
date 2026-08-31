#include "BossIdleState.h"
#include "Mob.h"
#include "Boss.h"
#include "BossStates/BossRunState.h"
#include "BossStates/BossPatrolState.h"
#include "Player.h"
#include <raymath.h>
#include <iostream>
#include <algorithm>
#include <cctype>

void BossIdleState::enter(Mob& mob) {
    std::string lowerType = mob.getMobType();
    std::transform(lowerType.begin(), lowerType.end(), lowerType.begin(), ::tolower);
    mob.setAnimation(lowerType + "_idle");
    mob.setVelocity({0, 0});
}

void BossIdleState::decideAction(Mob& mob) {
    Boss* boss = dynamic_cast<Boss*>(&mob);
    if (boss && boss->getIsWaitingForCutscene()) return;
    if (mob.getAggroCooldown() > 0.0f) return;

    Player* target = mob.getClosestPlayer();
    if (target) {
        float dist = Vector2Distance(mob.getPosition(), target->getPosition());
        if (dist <= mob.getConfig().aiData.detectionRange) {
            mob.changeState(std::make_unique<BossRunState>());
            return;
        }
    }
}

void BossIdleState::process(Mob& mob) {
    Boss* boss = dynamic_cast<Boss*>(&mob);
    if (boss && boss->getIsWaitingForCutscene()) return;

    mob.addStateTimer(GetFrameTime());
    // idle for a bit then patrol
    if (mob.getStateTimer() >= 2.0f) {
        mob.changeState(std::make_unique<BossPatrolState>());
    }
}

void BossIdleState::exit(Mob& mob) {
}
