#include "InputHandler.h"

void InputHandler::clearBindings() { keyBindings_.clear(); }

void InputHandler::bindKey(int key, std::unique_ptr<IPlayerCommand> command,
                           InputType type) {
  keyBindings_[key].push_back({std::move(command), type});
}

std::vector<IPlayerCommand *> InputHandler::handleInput() {
  std::vector<IPlayerCommand *> activeCommands;

  for (auto &pair : keyBindings_) {
    int key = pair.first;

    for (auto &binding : pair.second) {
      bool active = false;
      if (binding.type == InputType::PRESSED) {
        active = IsKeyPressed(key);
      } else if (binding.type == InputType::DOWN) {
        active = IsKeyDown(key);
      } else if (binding.type == InputType::RELEASED) {
        active = IsKeyReleased(key);
      }
      if (active) {
        activeCommands.push_back(binding.command.get());
      }
    }
  }

  return activeCommands;
}
