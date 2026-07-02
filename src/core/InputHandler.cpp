#include "InputHandler.h"

void InputHandler::clearBindings() {
  keyBindings_.clear();
}

void InputHandler::bindKey(int key, std::unique_ptr<IEntityCommand> command, bool isContinuous) {
  keyBindings_[key] = {std::move(command), isContinuous};
}

std::vector<IEntityCommand *> InputHandler::handleInput() {
  std::vector<IEntityCommand *> activeCommands;

  for (auto &pair : keyBindings_) {
    int key = pair.first;
    bool isContinuous = pair.second.isContinuous;

    bool active = isContinuous ? IsKeyDown(key) : IsKeyPressed(key);
    if (active) {
      activeCommands.push_back(pair.second.command.get());
    }
  }

  return activeCommands;
}
