#pragma once
#include "ISkill.h"

class Attack2Skill : public ISkill {
public:
    Attack2Skill(float mn = 0.0f, float dr = 0.5f) : ISkill(mn, dr) {
        animationName = "attack_2";
        nextComboSkillName = "Attack3";
    }

    void execute(Player& player) override;
};
