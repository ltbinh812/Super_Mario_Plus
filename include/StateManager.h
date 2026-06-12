#pragma once 
#include "GameState.h"
#include "IntroState.h"
#include "SettingState.h"
#include <stack>
#include <memory>

class StateManager {
    private: 
    std::stack<std::unique_ptr<GameState>> stateStack;

    public: 
    StateManager();
    void handleInput();
    void update(float dt);
    void processCommand();

    void pushState(std::unique_ptr<GameState> state);
    void popState();
    void changeState(std::unique_ptr<GameState> state);
    
    void render(float alpha) const;
};