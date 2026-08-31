#pragma once
#include "ISkill.h"

class DashSkill : public ISkill {
public:
    DashSkill(float mn = 0.0f, float dr = 0.2f) : ISkill(mn, dr) {
        animationName = "dash";
    }

    void execute(Player& player) override;
};
