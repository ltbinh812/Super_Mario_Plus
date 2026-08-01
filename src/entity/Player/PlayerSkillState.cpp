#include "PlayerSkillState.h"
#include "Player.h"

PlayerSkillState::PlayerSkillState(Player& player) : PlayerState(player) {
}

void PlayerSkillState::onEnter() {
    if (currentSkill) {
        // Deduct mana
        player.reduceMana(currentSkill->getManaCost());

        // Play skill animation
        player.playAnimation(currentSkill->getAnimationName());

        // Set timer for skill duration
        timer = currentSkill->getDuration();

        // Execute skill effect (velocity burst, etc.)
        currentSkill->execute(player);
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
