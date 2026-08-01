#pragma once
#include "PlayerState.h"

class PlayerFallState : public PlayerState {
    public: 

    PlayerFallState(Player& player);
    
    void onEnter() override;
    void onExit() override;
    void update(float dt) override;
    void onMoveLeft() override;
    void onMoveRight() override;
    void onStopLeft() override;
    void onStopRight() override;
    void onAttack() override;
};
