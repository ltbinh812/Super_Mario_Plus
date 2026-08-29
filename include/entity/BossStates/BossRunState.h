#pragma once
#include "IMobState.h"

class BossRunState : public IMobState {
public:
    void enter(Mob& mob) override;
    void decideAction(Mob& mob) override;
    void process(Mob& mob) override;
    void exit(Mob& mob) override;
    void onHitWall(Mob& mob, bool rightWall) override;
};
