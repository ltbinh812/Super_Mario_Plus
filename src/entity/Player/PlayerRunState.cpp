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
    player.changeState(player.jumpState);
}

void PlayerRunState::onCrouch() {
    player.crouch();
    player.changeState(player.crouchState);
}

void PlayerRunState::onAttack() {
    player.useSkill("normal_attack");
}

void PlayerRunState::onStopLeft() {
    player.stopLeftRun();
    if (player.getRuntimeStats().velocity.x == 0.0f) {
        player.changeState(player.idleState);
    }
}

void PlayerRunState::onStopRight() {
    player.stopRightRun();
    if (player.getRuntimeStats().velocity.x == 0.0f) {
        player.changeState(player.idleState);
    }
}