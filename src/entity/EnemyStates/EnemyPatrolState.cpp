#include "EnemyPatrolState.h"
#include "EnemyIdleState.h"
#include "../Mob.h"
#include <raylib.h>

EnemyPatrolState::EnemyPatrolState() 
    : patrolSpeed(0), patrolTime(0) {}

void EnemyPatrolState::enter(Mob& mob) {
    mob.setAnimation(mob.getMobType() + "_run");
    auto& aiData = mob.getConfig().aiData;
    patrolSpeed = aiData.patrolSpeed;
    patrolTime = aiData.patrolTime;
}

#include "EnemyRunState.h"
#include "Player.h"
#include <raymath.h>

void EnemyPatrolState::decideAction(Mob& mob) {
    Player* target = mob.getClosestPlayer();
    if (target) {
        float dist = Vector2Distance(mob.getPosition(), target->getPosition());
        if (dist <= mob.getConfig().aiData.detectionRange) {
            mob.changeState(std::make_unique<EnemyRunState>());
            return;
        }
    }
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
