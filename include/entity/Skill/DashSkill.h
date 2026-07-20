#pragma once
#include "ISkill.h"

class DashSkill : public ISkill {
public:
    DashSkill() {
        manaCost = 20.0f;
        duration = 0.3f;
        animationName = "dash";
    }

    void execute(Player& player) override;
};
