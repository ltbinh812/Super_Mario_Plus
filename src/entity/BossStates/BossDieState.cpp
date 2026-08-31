#include "BossStates/BossDieState.h"
#include "Mob.h"
#include <algorithm>
#include <cctype>

BossDieState::BossDieState() : dieTimer(0.0f) {}

void BossDieState::enter(Mob& mob) {
    std::string lowerType = mob.getMobType();
    std::transform(lowerType.begin(), lowerType.end(), lowerType.begin(), ::tolower);
    mob.setAnimation(lowerType + "_die");
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
