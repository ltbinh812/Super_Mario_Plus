#pragma once 
#include "ISkill.h"

class LongAttackSkill : public ISkill {
public:
    LongAttackSkill(float mn = 0.0f, float dr = 1.0f) : ISkill(mn, dr) {
        animationName = "long_attack";
    }

    void execute(Player& player) override;
};