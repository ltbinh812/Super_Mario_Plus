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

    // Apply friction to slide to a halt
    auto& vel = player.getRuntimeStatsMutable().velocity;
    float friction = 800.0f * dt;
    if (vel.x > 0) {
        vel.x = std::max(0.0f, vel.x - friction);
    } else if (vel.x < 0) {
        vel.x = std::min(0.0f, vel.x + friction);
    }

    if (invicibleTimer <= 0.0f) {
        if (player.getRuntimeStats().currentLiquid != CollisionType::None) {
            changePlayerState(player.swimState);
        } else if (player.getRuntimeStats().isOverlappingLadder) {
            changePlayerState(player.climbState);
        } else if (!player.getRuntimeStats().isGrounded) {
            changePlayerState(player.fallState);
        } else {
            changePlayerState(player.idleState);
        }
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
