#pragma once
#include "PlayerState.h"

class PlayerHurtState : public PlayerState {
    public:

    PlayerHurtState(Player& player);
    
    void onEnter() override;
    void onExit() override;
    void update(float dt) override;
    
    void onMoveLeft() override;
    void onMoveRight() override;
    void onStopLeft() override;
    void onStopRight() override;
    void onJump() override;
    void onClimb() override;
    bool canExit() const override { return false; }

    private:
    float invicibleTimer = 0.0f;
};
