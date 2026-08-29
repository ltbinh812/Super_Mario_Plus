#pragma once
#include "IMobState.h"

class BossIdleState : public IMobState {
public:
    void enter(Mob& mob) override;
    void decideAction(Mob& mob) override;
    void process(Mob& mob) override;
    void exit(Mob& mob) override;
};
