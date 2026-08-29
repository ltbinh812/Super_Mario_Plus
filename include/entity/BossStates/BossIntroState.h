#pragma once
#include "IMobState.h"

class BossIntroState : public IMobState {
private:
    bool isCutsceneFinished = false;
    
public:
    void enter(Mob& mob) override;
    void decideAction(Mob& mob) override;
    void process(Mob& mob) override;
    void exit(Mob& mob) override;
    
    void markCutsceneFinished() { isCutsceneFinished = true; }
};
