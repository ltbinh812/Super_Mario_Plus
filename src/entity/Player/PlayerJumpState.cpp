#include "PlayerJumpState.h"
#include "Player.h"

PlayerJumpState::PlayerJumpState(Player& player) : PlayerState(player) {}

void PlayerJumpState::onEnter() {
    player.playAnimation("jump");
}

void PlayerJumpState::onExit() {
}

void PlayerJumpState::update(float dt) {
}

void PlayerJumpState::onMoveLeft() {
    player.moveLeft();
}

void PlayerJumpState::onMoveRight() {
    player.moveRight();
}

void PlayerJumpState::onStopLeft() {
    player.stopLeftRun();
}

void PlayerJumpState::onStopRight() {
    player.stopRightRun();
}
