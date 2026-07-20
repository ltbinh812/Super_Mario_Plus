#include "PlayerState.h"
#include "Player.h"

PlayerState::PlayerState(Player& player) : player(player) {}

void PlayerState::changePlayerState(PlayerState& newState) {
    player.changeState(newState);
}
