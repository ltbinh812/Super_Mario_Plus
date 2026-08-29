#pragma once
#include "IMobState.h"

class BossPatrolState : public IMobState {
private:
    float patrolTimer;
    float currentPatrolTime;
    int direction;

public:
    BossPatrolState();
    void enter(Mob& mob) override;
    void decideAction(Mob& mob) override;
    void process(Mob& mob) override;
    void exit(Mob& mob) override;
};
