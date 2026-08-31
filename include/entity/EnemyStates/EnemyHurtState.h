#pragma once
#include "../IMobState.h"

class EnemyHurtState : public IMobState {
private:
    float hitstunDuration;
    float timer;
    float knockbackX;
public:
    EnemyHurtState(float knockbackX = 0.0f, float duration = 0.5f);
    void enter(Mob& mob) override;
    void decideAction(Mob& mob) override;
    void process(Mob& mob) override;
    void exit(Mob& mob) override;
};
