#pragma once
#include "ISkill.h"


class DashSkill : public ISkill {
    public:
        DashSkill();
        void execute(Player& player) override;
};