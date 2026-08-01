#pragma once
#include "ISkill.h"

class Punch4Skill : public ISkill {
public:
    Punch4Skill(float mn = 0.0f, float dr = 0.5f) : ISkill(mn, dr) {
        animationName = "punch4";
        // End of combo chain — no next skill
    }

    void execute(Player& player) override;
};
