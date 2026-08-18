#pragma once
#include "IBuffEffect.h"

class ShieldBuff : public IBuffEffect {
public:
    std::string getName() const override { return "Shield"; }
    std::string getFrameName() const override { return "item_shield_fix01 (Custom).png"; }
    float getDuration() const override { return 8.0f; }
    
    bool isInvincible() const override { return true; }

    std::unique_ptr<IBuffEffect> clone() const override {
        return std::make_unique<ShieldBuff>(*this);
    }
};
