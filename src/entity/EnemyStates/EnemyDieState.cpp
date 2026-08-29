#include "EnemyStates/EnemyDieState.h"
#include "Mob.h"

void EnemyDieState::enter(Mob& mob) {
    mob.setAnimation(mob.getMobType() + "_die");
    mob.setVelocity({ 0.0f, 0.0f });
    mob.setHitboxActive(false);
}

void EnemyDieState::decideAction(Mob& mob) {
    // Dead mobs make no decisions
}

void EnemyDieState::process(Mob& mob) {
    // Waiting for animation to finish is handled by Mob::getIsActive()
}

void EnemyDieState::exit(Mob& mob) {
}
