#include "GameState.h"

void GameState::PushCommand(Command&& cmd) {
    commandQueue.push_back(std::move(cmd));
}

std::vector<Command> GameState::ConsumeCommands() {
    std::vector<Command> result = std::move(commandQueue);
    commandQueue.clear(); // optional (sau move thì thường đã empty)
    return result;
}