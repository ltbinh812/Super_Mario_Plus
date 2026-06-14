#pragma once 
#include "Command.h"
#include <vector>

class StateManager;

class GameState {
    private:
    std::vector<Command> commandQueue;

    public:
    virtual ~GameState() = default;
    virtual void HandleInput() = 0;
    virtual void Update(float dt) = 0;
    virtual void Render(float alpha) const = 0;

    void PushCommand(Command&& cmd);
    std::vector<Command> ConsumeCommands();

};

