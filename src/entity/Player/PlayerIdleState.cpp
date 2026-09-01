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
    // Hồi mana đã chuyển lên Player::update() để CHẠY Ở MỌI TRẠNG THÁI.
    // Để ở đây thì đứng yên mới hồi, còn đang chạy/nhảy/bơi/leo thì không —
    // người chơi buộc phải đứng im giữa trận mới có mana dùng chiêu.
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
    player.useSkill("Attack1");
}

