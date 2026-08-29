#include "BossStates/BossSkillState.h"
#include "BossStates/BossRunState.h"
#include "BossStates/BossIdleState.h"
#include "BossStates/BossAttackState.h"
#include "Mob.h"
#include "Player.h"

void BossSkillState::enter(Mob& mob) {
    if (currentSkill) {
        mob.setAnimation(currentSkill->getSkillName()); // Chạy animation trùng tên skill (ví dụ: skill1)
        mob.setVelocity({0.0f, mob.getVelocity().y});
        mob.resetStateTimer();
        hasExecuted = false;
        
        Player* p = mob.getClosestPlayer();
        if (p) {
            mob.setFacingRight(p->getPosition().x > mob.getPosition().x);
        }
    }
}

void BossSkillState::decideAction(Mob& mob) {
    if (!currentSkill) return;
    
    // Nếu skill đã thi triển xong
    if (mob.getStateTimer() >= currentSkill->getDuration()) {
        int randVal = rand() % 100;
        
        // Random quyết định hành động tiếp theo
        if (randVal < 30) {
            // 30% tỷ lệ đứng nhìn ngầu (Idle)
            mob.changeState(std::make_unique<BossIdleState>());
        } else if (randVal < 60) {
            // 30% tỷ lệ đấm tiếp skill khác (Combo)
            mob.changeState(std::make_unique<BossAttackState>());
        } else {
            // 40% tỷ lệ tiếp tục di chuyển/đuổi theo (Run)
            mob.changeState(std::make_unique<BossRunState>());
        }
    }
}

void BossSkillState::process(Mob& mob) {
    if (!currentSkill) return;

    mob.addStateTimer(GetFrameTime());
    float elapsedTime = mob.getStateTimer();
    
    if (!hasExecuted && elapsedTime >= currentSkill->getHitboxStartTime() && elapsedTime <= currentSkill->getHitboxEndTime()) {
        currentSkill->execute(mob);
        hasExecuted = true;
    }
}

void BossSkillState::exit(Mob& mob) {
}
