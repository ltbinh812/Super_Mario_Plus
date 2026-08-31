#include "EnemyStates/EnemyRunState.h"
#include "EnemyStates/EnemyIdleState.h"
#include "EnemyStates/EnemyAttackState.h"
#include "EnemyStates/EnemySkillState.h"
#include "Mob.h"
#include "Player.h"
#include <cmath>
#include <raymath.h>

EnemyRunState::EnemyRunState()
    : speed(0), detectionRange(0), attackRange(0) {}

void EnemyRunState::enter(Mob& mob) {
    mob.setAnimation(mob.getMobType() + "_run");
    
    auto& aiData = mob.getConfig().aiData;
    speed = mob.getBaseStats().moveVelocity; // or use a dedicated run speed if we had one
    detectionRange = aiData.detectionRange;
    attackRange = aiData.attackRange;
}

void EnemyRunState::decideAction(Mob& mob) {
    Player* target = mob.getClosestPlayer();
    if (!target) {
        mob.changeState(std::make_unique<EnemyIdleState>());
        return;
    }

    float dist = Vector2Distance(mob.getPosition(), target->getPosition());
    if (dist > detectionRange) {
        mob.changeState(std::make_unique<EnemyIdleState>());
        return;
    }
    
    if (dist <= attackRange) {
        if (!mob.getEnemySkills().empty()) {
            int skillIndex = GetRandomValue(0, mob.getEnemySkills().size() - 1);
            mob.changeState(std::make_unique<EnemySkillState>(mob.getEnemySkills()[skillIndex].get()));
        } else {
            mob.changeState(std::make_unique<EnemyAttackState>());
        }
        return;
    }
}

void EnemyRunState::process(Mob& mob) {
    Player* target = mob.getClosestPlayer();
    if (target) {
        float dirX = target->getPosition().x - mob.getPosition().x;
        if (std::abs(dirX) < 5.0f) {
            mob.setVelocity({0.0f, mob.getVelocity().y});
        } else if (dirX > 0) {
            mob.setVelocity({speed, mob.getVelocity().y});
            mob.setFacingRight(true);
        } else {
            mob.setVelocity({-speed, mob.getVelocity().y});
            mob.setFacingRight(false);
        }
    }
}

void EnemyRunState::exit(Mob& mob) {
}

void EnemyRunState::onHitWall(Mob& mob, bool rightWall, bool isCliff) {
    if (isCliff) {
        mob.setAggroCooldown(2.0f);
        mob.changeState(std::make_unique<EnemyIdleState>());
        return;
    }
    if (mob.getRuntimeStats().isGrounded) {
        mob.setVelocity({mob.getVelocity().x, -mob.getBaseStats().jumpVelocity});
    }
}
