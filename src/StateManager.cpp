#include "StateManager.h"
#include <iostream> 

StateManager::StateManager() {
    std::unique_ptr<GameState> introState = std::make_unique<IntroState> ();
    stateStack.push(std::move(introState));
}

void StateManager::handleInput() {
    const auto &state = stateStack.top();
    state->handleInput();
}

void StateManager::update(float dt) {
    const auto &state = stateStack.top();
    state->update(dt);
}


void StateManager::processCommand() {
    const auto &state = stateStack.top();
    Command command = state->processCommand();

    if (command.type == CommandType::Setting) {
        std::cout << "ChangeState" << '\n';
        popState();
        stateStack.push(std::make_unique<SettingState>());
    }

    else if (command.type == CommandType::Intro) {
        std::cout << "ChangeState" << '\n';        
        popState();
        stateStack.push(std::make_unique<IntroState>());
    }
}

void StateManager::pushState(std::unique_ptr<GameState> state) {
    stateStack.push(std::move(state));
}

void StateManager::popState() {
    stateStack.pop();
}

void StateManager::render(float alpha) const {
    const auto &state = stateStack.top();
    state->render(alpha);
} 