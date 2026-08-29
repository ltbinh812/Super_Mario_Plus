#include "BossStates/BossHurtState.h"
#include "BossStates/BossRunState.h"
#include "BossStates/BossDieState.h"
#include "Mob.h"
#include <algorithm>
#include <cctype>

BossHurtState::BossHurtState() : hurtTimer(0.0f) {}

void BossHurtState::enter(Mob& mob) {
    std::string lowerType = mob.getMobType();
    std::transform(lowerType.begin(), lowerType.end(), lowerType.begin(), ::tolower);
    mob.setAnimation(lowerType + "_hurt");
    mob.setVelocity({0.0f, mob.getVelocity().y});
}

void BossHurtState::decideAction(Mob& mob) {
    if (mob.checkIsDead()) {
        mob.changeState(std::make_unique<BossDieState>());
        return;
    }

    // Nếu animation hurt kết thúc (hoặc sau 1 thời gian ngắn) thì tiếp tục đuổi theo
    if (hurtTimer >= 0.3f) { 
        mob.changeState(std::make_unique<BossRunState>());
    }
}

void BossHurtState::process(Mob& mob) {
    hurtTimer += GetFrameTime();
}

void BossHurtState::exit(Mob& mob) {
}
