#pragma once
#include "IBuffEffect.h"

class TimeStopBuff : public IBuffEffect {
public:
    std::string getName() const override { return "TimeStop"; }
    std::string getFrameName() const override { return "item_time_stop_fix01 (Custom).png"; }
    float getDuration() const override { return 5.0f; }
    
    bool canTimeStop() const override { return true; }

    std::unique_ptr<IBuffEffect> clone() const override {
        return std::make_unique<TimeStopBuff>(*this);
    }
};
