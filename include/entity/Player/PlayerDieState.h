#pragma once
#include "PlayerState.h"

class PlayerDieState : public PlayerState {
    public:

    PlayerDieState(Player& player);
    
    void onEnter() override;
    void onExit() override;
    void update(float dt) override;
    bool canExit() const override { return false; }
};
