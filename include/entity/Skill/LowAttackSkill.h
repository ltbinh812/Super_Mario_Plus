#pragma once
#include "ISkill.h"

class LowAttackSkill : public ISkill {
public:
    LowAttackSkill(float mn = 0.0f, float dr = 0.5f) : ISkill(mn, dr) {
        animationName = "low_attack";
    }

    void execute(Player& player) override;
};
