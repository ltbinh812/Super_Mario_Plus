#include "PlayerSkillState.h"
#include "Player.h"

PlayerSkillState::PlayerSkillState(Player& player) : PlayerState(player) {
}

void PlayerSkillState::onEnter() {
    if (currentSkill) {
        currentSkill->execute(player);
    }
}

void PlayerSkillState::onExit() {
    // Cleanup if needed
    currentSkill = nullptr;
}

void PlayerSkillState::update(float dt) {
    // The state update can check if the skill animation is finished,
    // and if so, transition back to Idle or Fall depending on grounded status.
    // E.g., if (animationEnded) player.changeState(player.idleState);
}
