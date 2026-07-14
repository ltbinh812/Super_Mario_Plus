#include "PlayerDieState.h"
#include "Player.h"

PlayerDieState::PlayerDieState(Player& player) : PlayerState(player) {}

void PlayerDieState::onEnter() {
    player.playAnimation("die");
}

void PlayerDieState::onExit() {
}

void PlayerDieState::update(float dt) {
}
