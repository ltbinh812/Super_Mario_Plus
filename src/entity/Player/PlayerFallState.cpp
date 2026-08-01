#include "PlayerFallState.h"
#include "Player.h"

PlayerFallState::PlayerFallState(Player& player) : PlayerState(player) {}

void PlayerFallState::onEnter() {
    player.playAnimation("fall");
}

void PlayerFallState::onExit() {
}

void PlayerFallState::update(float dt) {
}

void PlayerFallState::onMoveLeft() {
    player.moveLeft();
}

void PlayerFallState::onMoveRight() {
    player.moveRight();
}

void PlayerFallState::onStopLeft() {
    player.stopLeftRun();
}

void PlayerFallState::onStopRight() {
    player.stopRightRun();
}

void PlayerFallState::onAttack() {
    player.useSkill("jump_attack");
}