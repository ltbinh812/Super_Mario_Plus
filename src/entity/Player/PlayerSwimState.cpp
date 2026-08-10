#include "PlayerSwimState.h"
#include "Player.h"

PlayerSwimState::PlayerSwimState(Player& player) : PlayerState(player) {}

void PlayerSwimState::onEnter() {
    player.playAnimation("idle"); // Use idle anim until dedicated swim anim is added
    // Kill vertical velocity so player floats instead of sinking instantly
    player.getRuntimeStatsMutable().velocity.y = 0.0f;
}

void PlayerSwimState::onExit() {}

void PlayerSwimState::update(float dt) {
    // Apply water drag on horizontal axis each frame
    player.getRuntimeStatsMutable().velocity.x *= 0.92f;
    // Slow downward drift (buoyancy)
    player.getRuntimeStatsMutable().velocity.y *= 0.85f;
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

void PlayerSwimState::onJump() {
    // Surface: apply upward burst to exit water
    player.getRuntimeStatsMutable().velocity.y = player.getBaseStats().jumpVelocity * 0.6f;
}
