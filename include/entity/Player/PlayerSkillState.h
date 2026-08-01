#pragma once
#include "PlayerState.h"
#include "ISkill.h"

class PlayerSkillState : public PlayerState {
    private:
    ISkill* currentSkill = nullptr;
    ISkill* nextSkill = nullptr;  // Queued combo skill
    float timer = 0.0f;

    public:

    PlayerSkillState(Player& player);
    
    void setSkill(ISkill* skill) { currentSkill = skill; }

    void onEnter() override;
    void onExit() override;
    void update(float dt) override;
    bool canExit() const override { return timer <= 0.0f; }

    void onAttack() override;

    const ISkill* getCurrentSkill() const { return currentSkill; }
};
