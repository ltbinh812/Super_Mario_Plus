#pragma once

class Mob;

class IMobState {
public:
    virtual ~IMobState() = default;
    
    virtual void enter(Mob& mob) = 0;
    virtual void decideAction(Mob& mob) = 0;
    virtual void process(Mob& mob) = 0;
    virtual void exit(Mob& mob) = 0;
};
