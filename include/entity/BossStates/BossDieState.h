#pragma once
#include "IMobState.h"

class BossDieState : public IMobState {
private:
    float dieTimer;

public:
    BossDieState();
    void enter(Mob& mob) override;
    void decideAction(Mob& mob) override;
    void process(Mob& mob) override;
    void exit(Mob& mob) override;
};
