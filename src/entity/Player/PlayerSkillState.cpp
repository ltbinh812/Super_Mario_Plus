#include "PlayerSkillState.h"
#include "Player.h"

PlayerSkillState::PlayerSkillState(Player& player) : PlayerState(player) {
}

void PlayerSkillState::onEnter() {
    if (currentSkill) {
        // Stop horizontal movement when initiating a skill
        player.stopLeftRun();
        player.stopRightRun();

        // Deduct mana
        player.reduceMana(currentSkill->getManaCost());

        // Play skill animation, ensuring it doesn't loop so it holds the last frame during recovery
        player.playAnimation(currentSkill->getAnimationName(), false);

        // Set timer for skill duration plus recovery pause
        timer = currentSkill->getDuration() + currentSkill->getRecoveryDuration();
        hasExecuted = false; // Reset the flag for the new skill
    }
}

void PlayerSkillState::onExit() {
    currentSkill = nullptr;
    nextSkill = nullptr;
    timer = 0.0f;
}

void PlayerSkillState::update(float dt) {
    timer -= dt;
    timer = std::max(timer, 0.0f);

    float elapsedTime = getElapsedTime();
    if (!hasExecuted && elapsedTime >= currentSkill->getHitboxStartTime() &&
           elapsedTime <= currentSkill->getHitboxEndTime()) {
        // Execute skill effect (velocity burst, fireball spawn, etc.) ONLY ONCE per activation
        currentSkill->execute(player);
        hasExecuted = true;
    }


    if (timer == 0) {
        if (nextSkill) {
            // Chain into the next combo skill
            ISkill* next = nextSkill;
            nextSkill = nullptr;
            currentSkill = next;
            onEnter();  // Re-enter with the new skill
        } else {
            player.requestState(player.idleState);
        }
    }
}

void PlayerSkillState::onAttack() {
    if (!currentSkill || !currentSkill->hasNextCombo()) return;

    // Look up the next combo skill from the player's skill list
    ISkill* combo = player.findSkill(currentSkill->getNextComboSkillName());
    if (combo && player.hasEnoughMana(combo->getManaCost())) {
        nextSkill = combo;
    }
}

bool PlayerSkillState::isHitboxActive() const {
    if (!currentSkill) return false;
    float elapsedTime = getElapsedTime();
    return elapsedTime >= currentSkill->getHitboxStartTime() && 
           elapsedTime <= currentSkill->getHitboxEndTime();
}

float PlayerSkillState::getElapsedTime() const {
    if (!currentSkill) return 0.0f;
    float totalTime = currentSkill->getDuration() + currentSkill->getRecoveryDuration();
    return totalTime - timer;
}
