#include "InputHandler.h"

void InputHandler::clearBindings() { keyBindings_.clear(); }

void InputHandler::bindKey(int key, std::unique_ptr<IPlayerCommand> command,
                           int triggerMode) {
  keyBindings_[key].push_back({std::move(command), triggerMode});
}

std::vector<IPlayerCommand *> InputHandler::handleInput() {
  std::vector<IPlayerCommand *> activeCommands;

  for (auto &pair : keyBindings_) {
    int key = pair.first;

    for (auto &binding : pair.second) {
      bool active = false;
      if (binding.triggerMode == 1) active = IsKeyDown(key);
      else if (binding.triggerMode == 0) active = IsKeyReleased(key);
      else if (binding.triggerMode == 2) active = IsKeyPressed(key);
      
      if (active) {
        activeCommands.push_back(binding.command.get());
      }
    }
  }

  return activeCommands;
}
