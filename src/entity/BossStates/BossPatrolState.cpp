#include "BossStates/BossPatrolState.h"
#include "BossStates/BossRunState.h"
#include "BossStates/BossIdleState.h"
#include "Mob.h"
#include <cstdlib>

BossPatrolState::BossPatrolState() : patrolTimer(0.0f), currentPatrolTime(0.0f), direction(1) {}

void BossPatrolState::enter(Mob& mob) {
    mob.setAnimation("run"); // Dùng chung animation run cho lúc đi tuần
    currentPatrolTime = mob.getConfig().aiData.patrolTime;
    direction = (rand() % 2 == 0) ? 1 : -1;
    mob.setIsFacingRight(direction == 1);
}

void BossPatrolState::decideAction(Mob& mob) {
    if (mob.getTargetPlayers().empty()) return;

    for (auto* player : mob.getTargetPlayers()) {
        if (!player || player->isDead()) continue;

        float dist = Vector2Distance(mob.getPosition(), player->getPosition());
        if (dist <= mob.getConfig().aiData.detectionRange) {
            mob.changeState(std::make_unique<BossRunState>());
            return;
        }
    }
}

void BossPatrolState::process(Mob& mob) {
    patrolTimer += GetFrameTime();
    
    float speed = mob.getConfig().aiData.patrolSpeed;
    mob.setVelocity({speed * direction, mob.getVelocity().y});

    if (patrolTimer >= currentPatrolTime) {
        mob.changeState(std::make_unique<BossIdleState>());
    }
}

void BossPatrolState::exit(Mob& mob) {
}
