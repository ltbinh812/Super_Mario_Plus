#include "PlayerSwimState.h"
#include "Player.h"

PlayerSwimState::PlayerSwimState(Player& player) : PlayerState(player) {}

void PlayerSwimState::onEnter() {
    player.playAnimation("idle"); // Use idle anim until dedicated swim anim is added
}

void PlayerSwimState::onExit() {}

void PlayerSwimState::update(float dt) {
    // Apply water drag on horizontal axis each frame
    player.getRuntimeStatsMutable().velocity.x *= 0.80f;
    // Only apply strong vertical drag if moving up or already slow, so we don't kill the plunge momentum early
    if (player.getRuntimeStats().velocity.y <= 40.0f) {
        player.getRuntimeStatsMutable().velocity.y *= 0.80f;
    }
}

void PlayerSwimState::onMoveLeft() {
    player.swim(-1.0f);
}

void PlayerSwimState::onMoveRight() {
    player.swim(1.0f);
}

void PlayerSwimState::onStopLeft() {
    if (player.getRuntimeStats().velocity.x < 0)
        player.getRuntimeStatsMutable().velocity.x = 0;
}

void PlayerSwimState::onStopRight() {
    if (player.getRuntimeStats().velocity.x > 0)
        player.getRuntimeStatsMutable().velocity.x = 0;
}

void PlayerSwimState::onClimb() {
    // Swim UP
    player.swimY(-1.0f);
}

void PlayerSwimState::onCrouch() {
    // Swim DOWN
    player.swimY(1.0f);
}

void PlayerSwimState::onJump() {
    auto liquid = player.getRuntimeStats().currentLiquid;
    // Only allow jumping if the player's head is outside the liquid, OR if in poison/lava
    if (player.getRuntimeStats().isPartiallyOutsideLiquid || 
        liquid == CollisionType::Poison || liquid == CollisionType::Lava) {
        player.jump();
        changePlayerState(player.jumpState);
    }
}

void PlayerSwimState::onAttack() {
    player.useSkill("Punch1");
}
