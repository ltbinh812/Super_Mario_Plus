#pragma once
#include "PlayerState.h"

class PlayerClimbState : public PlayerState {
public:
    PlayerClimbState(Player& player);
    
    void onEnter() override;
    void onExit() override;
    void update(float dt) override;
    void onMoveLeft() override;
    void onMoveRight() override;
    void onJump() override;
    void onClimb() override;
    void onCrouch() override; // Leo xuống

    void onStopLeft() override {}
    void onStopRight() override {}
};
