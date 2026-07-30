#include "StateManager.h"
#include "Entity.h"
#include "GameState.h"
#include "IntroState.h"
#include "World01State.h"
#include "World02State.h"
#include "World03State.h"
#include <iostream>


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

StateManager::StateManager() {
  std::unique_ptr<GameState> initialState = std::make_unique<World03State>();
  stateStack.push(std::move(initialState));
}


StateManager::~StateManager() = default;

void StateManager::HandleInput() {
  if (stateStack.empty())
    return;

  const auto &state = stateStack.top();
  state->HandleInput();
}

void StateManager::Process() {
  if (stateStack.empty())
    return;

  const auto &state = stateStack.top();
  state->Process();

  auto commands = state->ConsumeCommands();
  for (auto &cmd : commands) {
    if (cmd) {
      cmd->Execute(*this);
    }
  }
}

void StateManager::Update(float dt) {
  if (stateStack.empty())
    return;

  const auto &state = stateStack.top();
  state->Update(dt);
}

void StateManager::Render(float alpha) const {
  if (stateStack.empty())
    return;

  const auto &state = stateStack.top();
  state->Render(alpha);
}