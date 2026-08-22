#pragma once
#include "IBuffEffect.h"

class InvisiBuff : public IBuffEffect {
public:
    std::string getName() const override { return "Invisibility"; }
    std::string getFrameName() const override { return "item_invisibility_drop"; }
    float getDuration() const override { return 6.0f; }
    
    // This is handled by visual rendering or enemy aggro system
    bool isInvincible() const override { return true; }

    std::unique_ptr<IBuffEffect> clone() const override {
        return std::make_unique<InvisiBuff>(*this);
    }
};
