#include "BossStates/BossRunState.h"
#include "BossStates/BossPatrolState.h"
#include "BossStates/BossAttackState.h"
#include "Mob.h"

void BossRunState::enter(Mob& mob) {
    mob.setAnimation("run");
}

void BossRunState::decideAction(Mob& mob) {
    if (mob.getTargetPlayers().empty()) {
        mob.changeState(std::make_unique<BossPatrolState>());
        return;
    }

    Player* closestPlayer = nullptr;
    float minDistance = 99999.0f;

    for (auto* player : mob.getTargetPlayers()) {
        if (!player || player->isDead()) continue;
        float dist = Vector2Distance(mob.getPosition(), player->getPosition());
        if (dist < minDistance) {
            minDistance = dist;
            closestPlayer = player;
        }
    }

    if (!closestPlayer) {
        mob.changeState(std::make_unique<BossPatrolState>());
        return;
    }

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
    Player* closestPlayer = nullptr;
    float minDistance = 99999.0f;

    for (auto* player : mob.getTargetPlayers()) {
        if (!player || player->isDead()) continue;
        float dist = Vector2Distance(mob.getPosition(), player->getPosition());
        if (dist < minDistance) {
            minDistance = dist;
            closestPlayer = player;
        }
    }

    if (closestPlayer) {
        float speed = mob.getConfig().moveVelocity;
        if (closestPlayer->getPosition().x < mob.getPosition().x) {
            mob.setVelocity({-speed, mob.getVelocity().y});
            mob.setIsFacingRight(false);
        } else {
            mob.setVelocity({speed, mob.getVelocity().y});
            mob.setIsFacingRight(true);
        }
    }
}

void BossRunState::exit(Mob& mob) {
}
