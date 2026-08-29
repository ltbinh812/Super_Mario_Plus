#pragma once
#include "IMobState.h"

class BossHurtState : public IMobState {
private:
    float hurtTimer;

public:
    BossHurtState();
    void enter(Mob& mob) override;
    void decideAction(Mob& mob) override;
    void process(Mob& mob) override;
    void exit(Mob& mob) override;
};
