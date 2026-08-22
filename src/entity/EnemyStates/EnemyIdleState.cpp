#include "EnemyIdleState.h"
#include "EnemyPatrolState.h"
#include "../Mob.h"
#include <raylib.h>

EnemyIdleState::EnemyIdleState(float idleTime) : duration(idleTime) {}

void EnemyIdleState::enter(Mob& mob) {
    mob.setAnimation(mob.getMobType() + "_idle");
    mob.setVelocity({0.0f, mob.getVelocity().y});
}

void EnemyIdleState::decideAction(Mob& mob) {
    // For now, just idle. Later, check for player aggro here.
}

void EnemyIdleState::process(Mob& mob) {
    mob.addStateTimer(GetFrameTime());
    if (mob.getStateTimer() >= duration) {
        mob.changeState(std::make_unique<EnemyPatrolState>());
    }
}

void EnemyIdleState::exit(Mob& mob) {
}
