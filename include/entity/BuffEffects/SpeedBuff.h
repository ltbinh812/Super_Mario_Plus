#pragma once
#include "IBuffEffect.h"

class SpeedBuff : public IBuffEffect {
public:
    std::string getName() const override { return "Speed"; }
    std::string getFrameName() const override { return "item_speed_fix01 (Custom).png"; }
    float getDuration() const override { return 10.0f; }
    
    // +50% Speed
    float getSpeedMultiplier() const override { return 0.5f; }

    std::unique_ptr<IBuffEffect> clone() const override {
        return std::make_unique<SpeedBuff>(*this);
    }
};
