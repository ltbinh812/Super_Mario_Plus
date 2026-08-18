#pragma once
#include "IBuffEffect.h"

class RandomBuff : public IBuffEffect {
public:
    std::string getName() const override { return "Random"; }
    std::string getFrameName() const override { return "item_random_fix01 (Custom).png"; }
    float getDuration() const override { return 0.0f; }

    std::unique_ptr<IBuffEffect> clone() const override {
        return std::make_unique<RandomBuff>(*this);
    }
};
