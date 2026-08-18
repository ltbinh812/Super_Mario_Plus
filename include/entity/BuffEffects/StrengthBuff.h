#pragma once
#include "IBuffEffect.h"

class StrengthBuff : public IBuffEffect {
public:
    std::string getName() const override { return "Strength"; }
    std::string getFrameName() const override { return "item_strength_fix01 (Custom).png"; }
    float getDuration() const override { return 10.0f; }
    
    // +100% Damage
    float getDamageMultiplier() const override { return 1.0f; }

    std::unique_ptr<IBuffEffect> clone() const override {
        return std::make_unique<StrengthBuff>(*this);
    }
};
