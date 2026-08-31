#pragma once
#include "../IMobState.h"

class EnemyIdleState : public IMobState {
private:
    float duration;
public:
    EnemyIdleState(float idleTime = 2.0f);
    void enter(Mob& mob) override;
    void decideAction(Mob& mob) override;
    void process(Mob& mob) override;
    void exit(Mob& mob) override;
};
