#pragma once
#include "ISkill.h"

class Punch1Skill : public ISkill {
public:
    Punch1Skill(float mn = 0.0f, float dr = 0.5f) : ISkill(mn, dr) {
        animationName = "punch1";
        nextComboSkillName = "Punch2";
    }

    void execute(Player& player) override;
};
  