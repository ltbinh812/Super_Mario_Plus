#pragma once
#include "IMobState.h"

class BossDebugInputState : public IMobState {
public:
    void enter(class Mob& mob) override;
    void decideAction(class Mob& mob) override;
    void process(class Mob& mob) override;
    void exit(class Mob& mob) override;
};
