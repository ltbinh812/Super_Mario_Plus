#pragma once
#include "IEntityState.h"
#include "Animation.h"

class Player;

class PlayerState : public IEntityState<Player> {
    protected: 
    Player& player;

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

};
