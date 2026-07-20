#pragma once
#include "ISkill.h"

class PunchSkill : public ISkill {
public:
    PunchSkill() {
        manaCost = 30.0f;
        duration = 0.5f;
        animationName = "punch1";
    }

    void execute(Player& player) override;
};
  