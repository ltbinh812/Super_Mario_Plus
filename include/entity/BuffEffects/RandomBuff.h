#pragma once
#include "IBuffEffect.h"

class RandomBuff : public IBuffEffect {
public:
    std::string getName() const override { return "Random"; }
    std::string getFrameName() const override { return "item_random_drop"; }
    int getFrameCount() const override { return 30; }
    float getFrameTime() const override { return 0.05f; } // Play at 20fps
    float getDuration() const override { return 0.0f; }

    std::unique_ptr<IBuffEffect> clone() const override {
        return std::make_unique<RandomBuff>(*this);
    }
};
