#pragma once
#include "ISkill.h"

class BlockSkill : public ISkill {
public:
    BlockSkill(float mn = 0.0f, float dr = 0.5f) : ISkill(mn, dr) {
        animationName = "block";
    }

    void execute(Player& player) override;
};
