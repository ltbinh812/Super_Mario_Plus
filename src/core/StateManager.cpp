#include "StateManager.h"
#include "GameState.h"
#include "IntroState.h"
#include <iostream> 

StateManager::StateManager() {
    std::unique_ptr<GameState> introState = std::make_unique<IntroState>();
    stateStack.push(std::move(introState));
}

StateManager::~StateManager() = default;

void StateManager::HandleInput() {
    if (stateStack.empty()) return;

    const auto &state = stateStack.top();
    state->HandleInput();
}

void StateManager::Update(float dt) {
    if (stateStack.empty()) return;

    const auto &state = stateStack.top();
    state->Update(dt);

    auto commands = state->ConsumeCommands();

    for (auto& cmd : commands) {
        ProcessCommand(std::move(cmd));
    }

}

void StateManager::ProcessCommand(Command command) {
    if (command.type == CommandType::None) return;
    else if (command.type == CommandType::Pop) PopState();
    else if (command.type == CommandType::Push) PushState(std::move(command.state));
    else if (command.type == CommandType::Change) ChangeState(std::move(command.state));
}

void StateManager::PushState(std::unique_ptr<GameState> state) {
    stateStack.push(std::move(state));
}

void StateManager::PopState() {
    if (!stateStack.empty())
    stateStack.pop();
}

void StateManager::ChangeState(std::unique_ptr<GameState> state) {
    if (!stateStack.empty())
    stateStack.pop();

    stateStack.push(std::move(state));
}

void StateManager::Render(float alpha) const {
    if (stateStack.empty()) return;
    
    const auto &state = stateStack.top();
    state->Render(alpha);
} 