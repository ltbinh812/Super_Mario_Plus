#include "PlayerHurtState.h"
#include "Player.h"

PlayerHurtState::PlayerHurtState(Player& player) : PlayerState(player) {}

void PlayerHurtState::onEnter() {
    player.playAnimation("hurt");
    invicibleTimer = 0.6f; // Set duration for hurt state
}

void PlayerHurtState::onExit() {
}

void PlayerHurtState::update(float dt) {
    invicibleTimer -= dt;
    if (invicibleTimer <= 0.0f) {
        changePlayerState(player.idleState);
    }
}

void PlayerHurtState::onMoveLeft() {
    player.moveLeft();
}

void PlayerHurtState::onMoveRight() {
    player.moveRight();
}

void PlayerHurtState::onStopLeft() {
    player.stopLeftRun();
}

void PlayerHurtState::onStopRight() {
    player.stopRightRun();
}

void PlayerHurtState::onJump() {
    auto liquid = player.getRuntimeStats().currentLiquid;
    if (player.getRuntimeStats().isGrounded || 
        liquid == CollisionType::Poison || liquid == CollisionType::Lava) {
        player.jump();
    }
}

void PlayerHurtState::onClimb() {
    auto liquid = player.getRuntimeStats().currentLiquid;
    if (liquid == CollisionType::Poison || liquid == CollisionType::Lava || liquid == CollisionType::Water) {
        player.swimY(-1.0f);
    }
}
