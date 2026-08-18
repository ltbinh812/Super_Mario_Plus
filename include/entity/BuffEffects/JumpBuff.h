#pragma once
#include "IBuffEffect.h"

class JumpBuff : public IBuffEffect {
public:
    std::string getName() const override { return "Jump"; }
    std::string getFrameName() const override { return "item_jump_fix01 (Custom).png"; }
    float getDuration() const override { return 8.0f; }
    
    // +50% Jump Power
    float getJumpMultiplier() const override { return 0.5f; }

    std::unique_ptr<IBuffEffect> clone() const override {
        return std::make_unique<JumpBuff>(*this);
    }
};
