#pragma once
#include "ISkill.h"

class Attack4Skill : public ISkill {
public:
    Attack4Skill(float mn = 0.0f, float dr = 0.5f) : ISkill(mn, dr) {
        animationName = "attack_4";
        // End of combo chain — no next skill
    }

    void execute(Player& player) override;
};
