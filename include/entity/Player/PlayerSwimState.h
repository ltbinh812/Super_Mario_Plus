#pragma once
#include "PlayerState.h"

class PlayerSwimState : public PlayerState {
private:
    float swimSoundTimer = 0.0f;
public:
    PlayerSwimState(Player& player);
    
    void onEnter() override;
    void onExit() override;
    void update(float dt) override;
    void onMoveLeft() override;
    void onMoveRight() override;
    void onJump() override;
    void onStopLeft() override;
    void onStopRight() override;
    void onClimb() override;
    void onCrouch() override;
    void onAttack() override;
};
