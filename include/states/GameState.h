#pragma once 
#include "IGameCommand.h"
#include <vector>
#include <memory>

class StateManager;

class GameState {
    private:
    std::vector<std::unique_ptr<IGameCommand>> stateCommandQueue;

    public:
    virtual ~GameState() = default;
    virtual void HandleInput() = 0;
    virtual void Process() = 0;
    virtual void Update(float dt) = 0;
    virtual void Render(float alpha) const = 0;

    void PushStateCommand(std::unique_ptr<IGameCommand> cmd);
    std::vector<std::unique_ptr<IGameCommand>> ConsumeCommands();

};

