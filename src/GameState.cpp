#include "GameState.h"

void GameState::pushCommand(Command&& cmd) {
    commandQueue.push_back(std::move(cmd));
}

std::vector<Command> GameState::consumeCommands() {
    std::vector<Command> result = std::move(commandQueue);
    commandQueue.clear(); // optional (sau move thì thường đã empty)
    return result;
}