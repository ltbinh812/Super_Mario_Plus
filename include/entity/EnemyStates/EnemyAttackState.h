#pragma once
#include "../IMobState.h"

class EnemyAttackState : public IMobState {
private:
    float windupTime;
    float activeTime;
    float recoveryTime;
    float timer;
    bool hasAttacked;
    int damage;
public:
    EnemyAttackState();
    void enter(Mob& mob) override;
    void decideAction(Mob& mob) override;
    void process(Mob& mob) override;
    void exit(Mob& mob) override;
};
