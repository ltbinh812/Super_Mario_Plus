#include "StateManager.h"
#include "GameState.h"
#include "IntroState.h"
#include <iostream> 

StateManager::StateManager() {
    std::unique_ptr<GameState> introState = std::make_unique<IntroState> ();
    stateStack.push(std::move(introState));
}

StateManager::~StateManager() = default;

void StateManager::handleInput() {
    if (stateStack.empty()) return;

    const auto &state = stateStack.top();
    state->handleInput();
}

void StateManager::update(float dt) {
    if (stateStack.empty()) return;

    const auto &state = stateStack.top();
    state->update(dt);

    auto commands = state->consumeCommands();

    for (auto& cmd : commands) {
        processCommand(std::move(cmd));
    }

}

void StateManager::processCommand(Command command) {
    if (command.type == CommandType::None) return;
    else if (command.type == CommandType::Pop) popState();
    else if (command.type == CommandType::Push) pushState(std::move(command.state));
    else if (command.type == CommandType::Change) changeState(std::move(command.state));
}

void StateManager::pushState(std::unique_ptr<GameState> state) {
    stateStack.push(std::move(state));
}

void StateManager::popState() {
    if (!stateStack.empty())
    stateStack.pop();
}

void StateManager::changeState(std::unique_ptr<GameState> state) {
    if (!stateStack.empty())
    stateStack.pop();

    stateStack.push(std::move(state));
}

void StateManager::render(float alpha) const {
    if (stateStack.empty()) return;
    
    const auto &state = stateStack.top();
    state->render(alpha);
} 