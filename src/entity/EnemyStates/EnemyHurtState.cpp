#include "EnemyStates/EnemyHurtState.h"
#include "EnemyStates/EnemyIdleState.h"
#include "Mob.h"
#include <iostream>

EnemyHurtState::EnemyHurtState(float knockbackX, float duration)
    : knockbackX(knockbackX), hitstunDuration(duration), timer(0.0f) {}

void EnemyHurtState::enter(Mob& mob) {
    mob.setAnimation(mob.getMobType() + "_hurt");
    mob.setVelocity({ knockbackX * 100.0f, -150.0f });
    mob.setHitboxActive(false); // disable hitbox while hurt
}

void EnemyHurtState::decideAction(Mob& mob) {
    if (timer >= hitstunDuration) {
        mob.changeState(std::make_unique<EnemyIdleState>());
    }
}

void EnemyHurtState::process(Mob& mob) {
    timer += GetFrameTime();
}

void EnemyHurtState::exit(Mob& mob) {
    // nothing to clean up
}
