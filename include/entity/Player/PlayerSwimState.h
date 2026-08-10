#pragma once
#include "PlayerState.h"

class PlayerSwimState : public PlayerState {
public:
    PlayerSwimState(Player& player);
    
    void onEnter() override;
    void onExit() override;
    void update(float dt) override;
    void onMoveLeft() override;
    void onMoveRight() override;
    void onJump() override; // Ngoi lên
    void onStopLeft() override;
    void onStopRight() override;
};
