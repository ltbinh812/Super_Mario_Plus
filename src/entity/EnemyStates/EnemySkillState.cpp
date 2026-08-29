#include "EnemySkillState.h"
#include "EnemyRunState.h"
#include "Mob.h"
#include "Player.h"

void EnemySkillState::enter(Mob& mob) {
    if (currentSkill) {
        // Stop horizontal movement
        mob.setVelocity({0, 0});
        
        // Play animation
        mob.setAnimation(currentSkill->getAnimName());
        
        timer = currentSkill->getDuration();
        hasExecuted = false;
        
        // Make sure mob is facing the closest player (optional but good for combat)
        Player* p = mob.getClosestPlayer();
        if (p) {
            mob.setFacingRight(p->getPosition().x > mob.getPosition().x);
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
    
    if (!hasExecuted && elapsedTime >= currentSkill->getHitboxStartTime() && elapsedTime <= currentSkill->getHitboxEndTime()) {
        currentSkill->execute(mob);
        hasExecuted = true;
    }

    if (elapsedTime >= currentSkill->getDuration()) {
        // Skill finished, transition back to Chase State
        mob.changeState(std::make_unique<EnemyRunState>());
    }
}

void EnemySkillState::exit(Mob& mob) {
    // Reset any state if needed
}
