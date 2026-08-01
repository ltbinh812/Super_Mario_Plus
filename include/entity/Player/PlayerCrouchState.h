#pragma once
#include "PlayerState.h"

class PlayerCrouchState : public PlayerState {
    public:

    PlayerCrouchState(Player& player);
    
    void onEnter() override;
    void onExit() override;
    void update(float dt) override;

    bool canExit() const override {return false;}
    void onAttack() override;
    void onStopCrouch() override;
};
