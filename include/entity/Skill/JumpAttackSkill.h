#pragma once
#include "ISkill.h"

class JumpAttackSkill : public ISkill {
public:
    JumpAttackSkill(float mn = 0.0f, float dr = 0.5f) : ISkill(mn, dr) {
        animationName = "jump_attack";
    }

    void execute(Player& player) override;
};
