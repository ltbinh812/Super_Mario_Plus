#include "BossStates/BossRunState.h"
#include "BossStates/BossPatrolState.h"
#include "BossStates/BossAttackState.h"
#include "Mob.h"

#include <raymath.h>
#include "Player.h"

void BossRunState::enter(Mob& mob) {
    mob.setAnimation("run");
}

void BossRunState::decideAction(Mob& mob) {
    Player* closestPlayer = mob.getClosestPlayer();

    if (!closestPlayer || closestPlayer->isDead()) {
        mob.changeState(std::make_unique<BossPatrolState>());
        return;
    }

    float minDistance = Vector2Distance(mob.getPosition(), closestPlayer->getPosition());

    if (minDistance > mob.getConfig().aiData.detectionRange) {
        mob.changeState(std::make_unique<BossPatrolState>());
        return;
    }

    // Khoảng cách attackRange thông thường
    if (minDistance <= mob.getConfig().aiData.attackRange) {
        mob.changeState(std::make_unique<BossAttackState>());
    } else if (minDistance <= 250.0f) { 
        // Cơ chế dùng skill đánh xa ngẫu nhiên nếu khoảng cách xa hơn attackRange một chút
        if (rand() % 100 < 2) { // Tỉ lệ thấp để không spam liên tục
            mob.changeState(std::make_unique<BossAttackState>());
        }
    }
}

void BossRunState::process(Mob& mob) {
    Player* closestPlayer = mob.getClosestPlayer();

    if (closestPlayer && !closestPlayer->isDead()) {
        float speed = mob.getConfig().aiData.patrolSpeed * 1.5f; // Run speed is usually faster than patrol
        if (closestPlayer->getPosition().x < mob.getPosition().x) {
            mob.setVelocity({-speed, mob.getVelocity().y});
            mob.setFacingRight(false);
        } else {
            mob.setVelocity({speed, mob.getVelocity().y});
            mob.setFacingRight(true);
        }
    }
}

void BossRunState::exit(Mob& mob) {
}
