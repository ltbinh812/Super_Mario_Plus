#include "PlayerIdleState.h"
#include "Player.h"

PlayerIdleState::PlayerIdleState(Player& player) : PlayerState(player) {}

void PlayerIdleState::onEnter() {
    player.playAnimation("idle");
}

void PlayerIdleState::onExit() {
}

void PlayerIdleState::update(float dt) {
}

void PlayerIdleState::onMoveLeft() {
    player.moveLeft();
    player.changeState(player.runState);
}

void PlayerIdleState::onMoveRight() {
    player.moveRight();
    player.changeState(player.runState);
}

void PlayerIdleState::onJump() {
    player.jump();
    player.changeState(player.jumpState);
}

void PlayerIdleState::onCrouch() {
    player.crouch();
    player.changeState(player.crouchState);
}

void PlayerIdleState::onAttack() {
    player.useSkill("normal_attack");
}

