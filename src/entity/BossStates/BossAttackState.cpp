#include "BossStates/BossAttackState.h"
#include "BossStates/BossIdleState.h"
#include "BossStates/BossSkillState.h"
#include "Mob.h"
#include <cstdlib>

void BossAttackState::enter(Mob& mob) {
    auto& skills = mob.getEnemySkills();
    if (skills.empty()) {
        mob.changeState(std::make_unique<BossIdleState>());
        return;
    }

    // Chọn ngẫu nhiên 1 skill từ danh sách
    int randomIndex = rand() % skills.size();
    
    // Chuyển sang BossSkillState để nó tự động load animation và hitbox tương ứng
    mob.changeState(std::make_unique<BossSkillState>(skills[randomIndex].get()));
}

void BossAttackState::decideAction(Mob& mob) {
}

void BossAttackState::process(Mob& mob) {
}

void BossAttackState::exit(Mob& mob) {
}
