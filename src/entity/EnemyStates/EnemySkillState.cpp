#include "EnemySkillState.h"
#include "EnemyRunState.h"
#include "Mob.h"
#include "Player.h"

void EnemySkillState::enter(Mob& mob) {
    if (currentSkill) {
        // Play animation
        mob.setAnimation(currentSkill->getAnimName());
        mob.resetStateTimer();
        
        timer = currentSkill->getDuration();
        hasExecuted = false;
        
        Player* p = mob.getClosestPlayer();
        float moveDir = 0.0f;
        if (p) {
            float dirX = p->getPosition().x - mob.getPosition().x;
            if (std::abs(dirX) > 5.0f) {
                bool faceRight = dirX > 0;
                mob.setFacingRight(faceRight);
            }
            moveDir = mob.getIsFacingRight() ? 1.0f : -1.0f;
        }
        
        // Cấp lực lao tới theo config của skill (giống boss)
        float dashMult = currentSkill->getDashMultiplier();
        if (dashMult > 0.0f) {
            mob.setVelocity({moveDir * mob.getBaseStats().moveVelocity * dashMult, mob.getVelocity().y});
        } else {
            mob.setVelocity({0.0f, mob.getVelocity().y});
        }
    }
}

void EnemySkillState::decideAction(Mob& mob) {
    // Enemy commits to the skill, no changing mind until it finishes
}

void EnemySkillState::process(Mob& mob) {
    if (!currentSkill) return;

    mob.addStateTimer(GetFrameTime());
    float elapsedTime = mob.getStateTimer();
    
    bool inHitboxWindow = elapsedTime >= currentSkill->getHitboxStartTime() && elapsedTime <= currentSkill->getHitboxEndTime();
    
    // Dừng lao tới khi bắt đầu ra đòn (giống boss)
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

    if (elapsedTime >= currentSkill->getDuration()) {
        // Skill finished, transition back to Chase State
        mob.setAttackCooldown(1.5f); // Prevent spamming attacks
        mob.changeState(std::make_unique<EnemyRunState>());
    }
}

void EnemySkillState::exit(Mob& mob) {
    if (currentSkill && currentSkill->emitsHitbox()) {
        mob.setHitboxActive(false);
    }
}
