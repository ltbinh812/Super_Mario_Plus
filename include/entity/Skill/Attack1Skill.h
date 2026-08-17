#pragma once
#include "ISkill.h"

class Attack1Skill : public ISkill {
public:
    Attack1Skill(float mn = 0.0f, float dr = 0.5f) : ISkill(mn, dr) {
        animationName = "attack_1";
        nextComboSkillName = "Attack2";
    }

    void execute(Player& player) override;
};
