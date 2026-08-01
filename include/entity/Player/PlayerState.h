#pragma once
#include "IEntityState.h"
#include "Animation.h"

class Player;

class PlayerState : public IEntityState<Player> {
    protected: 
    Player& player;

    // Protected helper so subclasses can trigger unconditional state changes.
    // PlayerState is a friend of Player, so this compiles; subclasses inherit access.
    void changePlayerState(PlayerState& newState);

    public: 
    virtual ~PlayerState() = default;

    PlayerState(Player& player);
    virtual void onMoveRight() {}
    virtual void onMoveLeft() {}
    virtual void onJump() {}
    virtual void onCrouch() {}
    virtual void onAttack() {}
    virtual void onStopLeft() {}
    virtual void onStopRight() {}
    virtual void onStopCrouch() {}

    virtual bool canExit() const { return true; }

};
