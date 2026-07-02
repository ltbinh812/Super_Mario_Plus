#pragma once 
#include <stack>
#include <memory>
#include "IGameCommand.h"

class GameState;

class StateManager {
    private: 
    std::stack<std::unique_ptr<GameState>> stateStack;

public: 
    void PushState(std::unique_ptr<GameState> state);
    void PopState();
    void ChangeState(std::unique_ptr<GameState> state);

    StateManager();
    ~StateManager();
    bool isEmpty() const { return stateStack.empty(); }
    void HandleInput();
    void Process();
    void Update(float dt);
    void Render(float alpha) const;
};