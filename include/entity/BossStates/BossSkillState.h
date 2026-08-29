#pragma once
#include "IMobState.h"
#include "Skill/IEnemySkill.h"

class BossSkillState : public IMobState {
private:
    IEnemySkill* currentSkill = nullptr;
    float timer = 0.0f;
    bool hasExecuted = false;

public:
    BossSkillState(IEnemySkill* skill) : currentSkill(skill) {}

    void enter(Mob& mob) override;
    void decideAction(Mob& mob) override;
    void process(Mob& mob) override;
    void exit(Mob& mob) override;
};
