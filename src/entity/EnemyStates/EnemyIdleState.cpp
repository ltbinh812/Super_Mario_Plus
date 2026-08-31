#include "EnemyIdleState.h"
#include "EnemyPatrolState.h"
#include "../Mob.h"
#include <raylib.h>

EnemyIdleState::EnemyIdleState(float idleTime) : duration(idleTime) {}

void EnemyIdleState::enter(Mob& mob) {
    mob.setAnimation(mob.getMobType() + "_idle");
    mob.setVelocity({0.0f, mob.getVelocity().y});
    mob.resetStateTimer();
}

#include "EnemyRunState.h"
#include "Player.h"
#include <raymath.h>

void EnemyIdleState::decideAction(Mob& mob) {
    if (mob.getAggroCooldown() > 0.0f) return;
    Player* target = mob.getClosestPlayer();
    if (target) {
        float dist = Vector2Distance(mob.getPosition(), target->getPosition());
        if (dist <= mob.getConfig().aiData.detectionRange) {
            mob.changeState(std::make_unique<EnemyRunState>());
            return;
        }
    }
}

void EnemyIdleState::process(Mob& mob) {
    mob.addStateTimer(GetFrameTime());
    if (mob.getStateTimer() >= duration) {
        mob.changeState(std::make_unique<EnemyPatrolState>());
    }
}

void EnemyIdleState::exit(Mob& mob) {
}
