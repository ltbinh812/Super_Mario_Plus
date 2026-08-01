#pragma once
#include "ISkill.h"

class Punch2Skill : public ISkill {
public:
    Punch2Skill(float mn = 0.0f, float dr = 0.5f) : ISkill(mn, dr) {
        animationName = "punch2";
        nextComboSkillName = "Punch3";
    }

    void execute(Player& player) override;
};
