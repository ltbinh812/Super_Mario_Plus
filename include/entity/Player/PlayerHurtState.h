#pragma once
#include "PlayerState.h"

class PlayerHurtState : public PlayerState {
    public:

    PlayerHurtState(Player& player);
    
    void onEnter() override;
    void onExit() override;
    void update(float dt) override;
    bool canExit() const override { return false; }

};
