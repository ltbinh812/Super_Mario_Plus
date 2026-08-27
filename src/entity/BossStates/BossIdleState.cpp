#include "BossIdleState.h"
#include "Mob.h"
#include <iostream>

void BossIdleState::enter(Mob& mob) {
    mob.setAnimation("idle");
    mob.setVelocity({0, 0});
}

void BossIdleState::decideAction(Mob& mob) {
    // Do nothing. Boss waits for Cutscene trigger via Boss::onCutsceneStart.
}

void BossIdleState::process(Mob& mob) {
    // Process nothing.
}

void BossIdleState::exit(Mob& mob) {
}
