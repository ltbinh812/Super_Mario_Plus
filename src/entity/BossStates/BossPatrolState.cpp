#include "BossStates/BossPatrolState.h"
#include "BossStates/BossRunState.h"
#include "BossStates/BossIdleState.h"
#include "Mob.h"
#include <cstdlib>
#include <raymath.h>
#include "Player.h"

BossPatrolState::BossPatrolState() : patrolTimer(0.0f), currentPatrolTime(0.0f), direction(1) {}

void BossPatrolState::enter(Mob& mob) {
    mob.setAnimation("run"); // Dùng chung animation run cho lúc đi tuần
    currentPatrolTime = mob.getConfig().aiData.patrolTime;
    direction = (rand() % 2 == 0) ? 1 : -1;
    mob.setFacingRight(direction == 1);
}

void BossPatrolState::decideAction(Mob& mob) {
    Player* closestPlayer = mob.getClosestPlayer();
    if (!closestPlayer || closestPlayer->isDead()) return;

    float dist = Vector2Distance(mob.getPosition(), closestPlayer->getPosition());
    if (dist <= mob.getConfig().aiData.detectionRange) {
        mob.changeState(std::make_unique<BossRunState>());
    }
}

void BossPatrolState::process(Mob& mob) {
    patrolTimer += GetFrameTime();
    
    float speed = mob.getConfig().aiData.patrolSpeed;
    float vx = mob.getIsFacingRight() ? speed : -speed;
    mob.setVelocity({vx, mob.getVelocity().y});

    if (patrolTimer >= currentPatrolTime) {
        mob.changeState(std::make_unique<BossIdleState>());
    }
}

void BossPatrolState::exit(Mob& mob) {
}

void BossPatrolState::onHitWall(Mob& mob, bool rightWall) {
    mob.setFacingRight(!mob.getIsFacingRight());
}
