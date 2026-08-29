#include "BossStates/BossSkillState.h"
#include "BossStates/BossRunState.h"
#include "BossStates/BossIdleState.h"
#include "BossStates/BossAttackState.h"
#include "BossStates/BossDebugInputState.h"
#include "Mob.h"
#include "Player.h"

void BossSkillState::enter(Mob& mob) {
    if (currentSkill) {
        mob.setAnimation(currentSkill->getAnimName()); // Chạy animation trùng tên skill (ví dụ: skill1)
        mob.resetStateTimer();
        hasExecuted = false;
        
        Player* p = mob.getClosestPlayer();
        float moveDir = 0.0f;
        if (p) {
            bool faceRight = p->getPosition().x > mob.getPosition().x;
            mob.setFacingRight(faceRight);
            moveDir = faceRight ? 1.0f : -1.0f;
        }
        
        // Cấp 1 lực lao tới người chơi tùy theo config của skill
        float dashMult = currentSkill->getDashMultiplier();
        mob.setVelocity({moveDir * mob.getBaseStats().moveVelocity * dashMult, mob.getVelocity().y});
    }
}

void BossSkillState::decideAction(Mob& mob) {
    if (!currentSkill) return;
    
    // Nếu skill đã thi triển xong
    if (mob.getStateTimer() >= currentSkill->getDuration()) {
        if (returnToDebugMode) {
            mob.changeState(std::make_unique<BossDebugInputState>());
            return;
        }

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
    
    bool inHitboxWindow = elapsedTime >= currentSkill->getHitboxStartTime() && elapsedTime <= currentSkill->getHitboxEndTime();
    
    // Dừng lao tới khi bắt đầu ra đòn thực sự (tạo cảm giác lực đâm mạnh và cắm chân xuống)
    if (elapsedTime >= currentSkill->getHitboxStartTime()) {
        mob.setVelocity({0.0f, mob.getVelocity().y});
    }
    
    if (inHitboxWindow) {
        if (currentSkill->emitsHitbox()) {
            mob.setHitboxActive(true, currentSkill->getHitbox(mob));
        }
        if (!hasExecuted) {
            currentSkill->execute(mob);
            hasExecuted = true;
        }
    } else {
        if (currentSkill->emitsHitbox()) {
            mob.setHitboxActive(false);
        }
    }
}

void BossSkillState::exit(Mob& mob) {
    if (currentSkill && currentSkill->emitsHitbox()) {
        mob.setHitboxActive(false);
    }
}
