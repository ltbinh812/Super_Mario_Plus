#pragma once
#include "../IMobState.h"

class EnemyPatrolState : public IMobState {
private:
    float patrolSpeed;
    float patrolTime;
public:
    EnemyPatrolState();
    void enter(Mob& mob) override;
    void decideAction(Mob& mob) override;
    void process(Mob& mob) override;
    void exit(Mob& mob) override;
};
