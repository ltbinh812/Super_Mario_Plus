#include "PlayerHurtState.h"
#include "Player.h"

PlayerHurtState::PlayerHurtState(Player& player) : PlayerState(player) {}

void PlayerHurtState::onEnter() {
    player.playAnimation("hurt");
}

void PlayerHurtState::onExit() {
}

void PlayerHurtState::update(float dt) {
}
