#pragma once
#include "IBuffEffect.h"

class GoldMagnetBuff : public IBuffEffect {
public:
    std::string getName() const override { return "GoldMagnet"; }
    std::string getFrameName() const override { return "item_gold_magnet_fix01 (Custom).png"; }
    float getDuration() const override { return 12.0f; }
    
    bool hasGoldMagnet() const override { return true; }

    std::unique_ptr<IBuffEffect> clone() const override {
        return std::make_unique<GoldMagnetBuff>(*this);
    }
};
