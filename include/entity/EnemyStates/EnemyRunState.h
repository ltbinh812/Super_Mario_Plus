#pragma once
#include "../IMobState.h"

class EnemyRunState : public IMobState {
private:
    float speed;
    float detectionRange;
    float attackRange;
public:
    EnemyRunState();
    void enter(Mob& mob) override;
    void decideAction(Mob& mob) override;
    void process(Mob& mob) override;
    void exit(Mob& mob) override;
    void onHitWall(Mob& mob, bool rightWall, bool isCliff = false) override;
};
