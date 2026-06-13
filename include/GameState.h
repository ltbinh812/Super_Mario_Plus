#pragma once 
#include <queue>
#include "Command.h"
#include <vector>

class StateManager;

class GameState {
    private:
    std::vector<Command> commandQueue;

    public:
    virtual ~GameState() = default;
    virtual void handleInput() = 0;
    virtual void update(float dt) = 0;
    virtual void render(float alpha) const = 0;

    void pushCommand(Command cmd);
    std::vector<Command> consumeCommands();

};

