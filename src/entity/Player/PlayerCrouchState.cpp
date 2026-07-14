#include "PlayerCrouchState.h"
#include "Player.h"

PlayerCrouchState::PlayerCrouchState(Player& player) : PlayerState(player) {}

void PlayerCrouchState::onEnter() {
    player.playAnimation("crouch");
}

void PlayerCrouchState::onExit() {
}

void PlayerCrouchState::update(float dt) {
}
