#pragma once
#include "PlayerState.h"

class PlayerJumpState : public PlayerState {
    public:

    PlayerJumpState(Player& player);
    
    void onEnter() override;
    void onExit() override;
    void update(float dt) override;
    void onMoveLeft() override;
    void onMoveRight() override;
    void onStopLeft() override;
    void onStopRight() override;

};
