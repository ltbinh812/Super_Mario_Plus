#include "PlayerIdleState.h"
#include "Player.h"

PlayerIdleState::PlayerIdleState(Player& player) : PlayerState(player) {}

void PlayerIdleState::onEnter() {
    player.idle();
    player.playAnimation("idle");
}

void PlayerIdleState::onExit() {
}

void PlayerIdleState::update(float dt) {
    player.increaseMana(1000 * dt);
}

void PlayerIdleState::onMoveLeft() {
    player.moveLeft();
    changePlayerState(player.runState);
}

void PlayerIdleState::onMoveRight() {
    player.moveRight();
    changePlayerState(player.runState);
}

void PlayerIdleState::onJump() {
    player.jump();
    changePlayerState(player.jumpState);
}

void PlayerIdleState::onCrouch() {
    player.crouch();
    changePlayerState(player.crouchState);
}

void PlayerIdleState::onAttack() {
    player.useSkill("normal_attack");
}

