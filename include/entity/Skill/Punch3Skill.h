#pragma once
#include "ISkill.h"

class Punch3Skill : public ISkill {
public:
    Punch3Skill(float mn = 0.0f, float dr = 0.5f) : ISkill(mn, dr) {
        animationName = "punch3";
        nextComboSkillName = "Punch4";
    }

    void execute(Player& player) override;
};
