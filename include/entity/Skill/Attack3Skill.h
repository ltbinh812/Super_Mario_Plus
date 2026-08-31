#pragma once
#include "ISkill.h"

class Attack3Skill : public ISkill {
public:
    Attack3Skill(float mn = 0.0f, float dr = 0.5f) : ISkill(mn, dr) {
        animationName = "attack_3";
        nextComboSkillName = "Attack4";
    }

    void execute(Player& player) override;
};
