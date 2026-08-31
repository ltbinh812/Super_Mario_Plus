#include "PlayerRunState.h"
#include "Player.h"

PlayerRunState::PlayerRunState(Player& player) : PlayerState(player) {}

void PlayerRunState::onEnter() {
    player.playAnimation("run");    
}

void PlayerRunState::onExit() {
}

void PlayerRunState::update(float dt) {
}

void PlayerRunState::onMoveLeft() {
    player.moveLeft();
}

void PlayerRunState::onMoveRight() {
    player.moveRight();
}

void PlayerRunState::onJump() {
    player.jump();
    changePlayerState(player.jumpState);
}

void PlayerRunState::onCrouch() {
    player.crouch();
    changePlayerState(player.crouchState);
}

void PlayerRunState::onAttack() {
    player.useSkill("Attack1");
}

void PlayerRunState::onStopLeft() {
    player.stopLeftRun();
}

void PlayerRunState::onStopRight() {
    player.stopRightRun();
}