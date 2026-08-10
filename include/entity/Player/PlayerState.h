#pragma once
#include "IEntityState.h"
#include "Animation.h"

class Player;

// PlayerState is the concrete base for all Player states.
// It inherits all input hooks from IEntityState<Player> — do NOT re-declare them here.
class PlayerState : public IEntityState<Player> {
protected:
    Player& player;

    // Protected helper — lets subclasses request state changes via the public Player API
    void changePlayerState(PlayerState& newState);

public:
    virtual ~PlayerState() = default;
    PlayerState(Player& player);
};
