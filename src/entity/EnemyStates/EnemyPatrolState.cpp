#include "EnemyPatrolState.h"
#include "EnemyIdleState.h"
#include "../Mob.h"
#include <raylib.h>

EnemyPatrolState::EnemyPatrolState(float speed, float time) 
    : patrolSpeed(speed), patrolTime(time) {}

void EnemyPatrolState::enter(Mob& mob) {
    mob.setAnimation(mob.getMobType() + "_run");
}

void EnemyPatrolState::decideAction(Mob& mob) {
    // Later: Check for player aggro here
}

void EnemyPatrolState::process(Mob& mob) {
    mob.addStateTimer(GetFrameTime());
    
    float vx = mob.getIsFacingRight() ? patrolSpeed : -patrolSpeed;
    mob.setVelocity({vx, mob.getVelocity().y});
    
    if (mob.getStateTimer() >= patrolTime) {
        mob.changeState(std::make_unique<EnemyIdleState>());
    }
}

void EnemyPatrolState::exit(Mob& mob) {
}
