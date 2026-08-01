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
