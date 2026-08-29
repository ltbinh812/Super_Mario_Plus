#include "BossStates/BossDieState.h"
#include "Mob.h"

BossDieState::BossDieState() : dieTimer(0.0f) {}

void BossDieState::enter(Mob& mob) {
    mob.setAnimation("die");
    mob.setVelocity({0.0f, mob.getVelocity().y});
    mob.setHitboxActive(false);
}

void BossDieState::decideAction(Mob& mob) {
    // Boss đã chết, không làm gì cả
}

void BossDieState::process(Mob& mob) {
    dieTimer += GetFrameTime();
}

void BossDieState::exit(Mob& mob) {
}
