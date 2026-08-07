#include "PlayerCrouchState.h"
#include "Player.h"

PlayerCrouchState::PlayerCrouchState(Player& player) : PlayerState(player) {}

void PlayerCrouchState::onEnter() {
    player.playAnimation("crouch");
    player.crouch();
}

void PlayerCrouchState::onExit() {
    player.standUp();
}

void PlayerCrouchState::update(float dt) {
}

void PlayerCrouchState::onAttack() {
    player.useSkill("Punch1");
}

void PlayerCrouchState::onJump() {
    player.jump();
    changePlayerState(player.jumpState);
}

void PlayerCrouchState::onStopCrouch() {
    changePlayerState(player.idleState);
}

void PlayerCrouchState::onMoveLeft() {
    player.moveLeft();
}

void PlayerCrouchState::onMoveRight() {
    player.moveRight();
}

void PlayerCrouchState::onStopLeft() {
    player.stopLeftRun();
}

void PlayerCrouchState::onStopRight() {
    player.stopRightRun();
}