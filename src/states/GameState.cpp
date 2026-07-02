#include "GameState.h"

void GameState::PushStateCommand(std::unique_ptr<IGameCommand> cmd) {
    stateCommandQueue.push_back(std::move(cmd));
}

std::vector<std::unique_ptr<IGameCommand>> GameState::ConsumeCommands() {
    std::vector<std::unique_ptr<IGameCommand>> result = std::move(stateCommandQueue);
    stateCommandQueue.clear(); // optional (sau move thì thường đã empty)
    return result;
}