#pragma once
#include "PlayerState.h"
#include "ISkill.h"

class PlayerSkillState : public PlayerState {
    private:
    ISkill* currentSkill = nullptr;

    public:

    PlayerSkillState(Player& player);
    
    void setSkill(ISkill* skill) { currentSkill = skill; }

    void onEnter() override;
    void onExit() override;
    void update(float dt) override;

};
