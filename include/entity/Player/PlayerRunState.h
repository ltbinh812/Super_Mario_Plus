#pragma once
#include "PlayerState.h"

class PlayerRunState : public PlayerState {
    public:

    PlayerRunState(Player& player);
    
    void onEnter() override;
    void onExit() override;
    void update(float dt) override;
    void onMoveLeft() override;
    void onMoveRight() override;
    void onJump() override;
    void onCrouch() override;
    void onAttack() override;
    void onStopLeft() override;
    void onStopRight() override;

};
