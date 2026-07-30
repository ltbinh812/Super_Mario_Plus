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
    timer = 0.0f;
}

void PlayerSkillState::update(float dt) {
    timer -= dt;
    timer = std::max(timer, 0.0f);

    // Nếu đang dùng skill trên thang và không phải skill di chuyển (như Dash), giữ nhân vật đứng yên trên thang
    if (player.getRuntimeStats().isOverlappingLadder) {
        if (player.getRuntimeStats().velocity.x == 0.0f) {
            player.getRuntimeStatsMutable().velocity.y = 0.0f;
        }
    }

    if (timer == 0) {
        player.updateStateFromPhysics();
    }
}

