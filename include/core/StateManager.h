#pragma once 
#include <stack>
#include <memory>
#include "Command.h"

class GameState;

class StateManager {
    private: 
    std::stack<std::unique_ptr<GameState>> stateStack;

    void PushState(std::unique_ptr<GameState> state);
    void PopState();
    void ChangeState(std::unique_ptr<GameState> state);

    public: 
    StateManager();
    ~StateManager();
    void HandleInput();
    void Update(float dt);
    void ProcessCommand(Command command);
    void Render(float alpha) const;
};