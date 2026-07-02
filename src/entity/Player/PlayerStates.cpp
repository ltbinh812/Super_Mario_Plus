#include "Player.h"
#include "PlayerStates.h"

// Idle State
void PlayerIdleState::onJump(Player& player) {
    player.setRequest(player.getJumpState());
}

void PlayerIdleState::onMoveRight(Player& player) {
    player.setRequest(player.getRunState());
    player.setFaceDirection(true);
}

void PlayerIdleState::onMoveLeft(Player& player) {
    player.setRequest(player.getRunState());
    player.setFaceDirection(false);
}

void PlayerIdleState::update(Player& player, float dt) {}

void PlayerIdleState::onEnter(Player& player) {
    // Không ép velocityX = 0 nữa để lực quán tính (hoặc Dash) được duy trì
    player.setAnimation(player.getIdleAnimation());
}

// Run State
void PlayerRunState::onJump(Player& player) {
    player.setRequest(player.getJumpState());
}

void PlayerRunState::onMoveRight(Player& player) {
    player.setFaceDirection(true);
}

void PlayerRunState::onMoveLeft(Player& player) {
    player.setFaceDirection(false);
}

void PlayerRunState::onStopMove(Player& player) {
    player.setRequest(player.getIdleState());
}

void PlayerRunState::update(Player& player, float dt) {}

void PlayerRunState::onEnter(Player& player) {
    player.setAnimation(player.getRunAnimation());
    // Lực chạy sẽ được cộng dồn bằng gia tốc trong Player::update
}

// Jump State
void PlayerJumpState::onMoveRight(Player& player) {
    player.setFaceDirection(true);
}

void PlayerJumpState::onMoveLeft(Player& player) {
    player.setFaceDirection(false);
}

void PlayerJumpState::update(Player& player, float dt) {
    if (player.checkIsGrounded()) {
        player.setRequest(player.getIdleState());
    }
}

void PlayerJumpState::onEnter(Player& player) {
    player.setAnimation(player.getJumpAnimation());
    player.setVelocityY(player.getStats().jumpForce);
    player.setIsGrounded(false);
}

