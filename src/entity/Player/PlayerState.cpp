#include "PlayerState.h"
#include "Player.h"

PlayerState::PlayerState(Player& player) : player(player) {}

void PlayerState::changePlayerState(PlayerState& newState) {
    // Use public requestState() — respects canExit() guard
    player.requestState(newState);
}
