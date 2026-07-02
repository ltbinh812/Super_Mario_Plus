#pragma once
#include "IGameCommand.h"
#include "StateManager.h"
#include "GameState.h"
#include <memory>

class PushStateCommand : public IGameCommand {
private:
    std::unique_ptr<GameState> state;

public:
    PushStateCommand(std::unique_ptr<GameState> newState) : state(std::move(newState)) {}

    void Execute(StateManager& manager) override {
        manager.PushState(std::move(state));
    }
};

class PopStateCommand : public IGameCommand {
public:
    void Execute(StateManager& manager) override {
        manager.PopState();
    }
};

class ChangeStateCommand : public IGameCommand {
private:
    std::unique_ptr<GameState> state;

public:
    ChangeStateCommand(std::unique_ptr<GameState> newState) : state(std::move(newState)) {}

    void Execute(StateManager& manager) override {
        manager.ChangeState(std::move(state));
    }
};