#pragma once 
#include "ISkill.h"

class SpecialSkillAttack : public ISkill {
public:
    SpecialSkillAttack(float mn = 0.0f, float dr = 1.0f) : ISkill(mn, dr) {
        animationName = "special_attack";
    }

    void execute(Player& player) override;
};
