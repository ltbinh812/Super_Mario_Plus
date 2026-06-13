#pragma once 
#include <stack>
#include <memory>
#include "Command.h"

class GameState;

class StateManager {
    private: 
    std::stack<std::unique_ptr<GameState>> stateStack;

    void pushState(std::unique_ptr<GameState> state);
    void popState();
    void changeState(std::unique_ptr<GameState> state);

    public: 
    StateManager();
    ~StateManager();
    void handleInput();
    void update(float dt);
    void processCommand(Command command);
    void render(float alpha) const;
};