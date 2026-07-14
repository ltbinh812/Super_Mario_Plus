#include "InputHandler.h"

void InputHandler::clearBindings() {
  keyBindings_.clear();
}

void InputHandler::bindKey(int key, std::unique_ptr<IPlayerCommand> command, bool isPressed) {
  keyBindings_[key].push_back({std::move(command), isPressed});
}

std::vector<IPlayerCommand *> InputHandler::handleInput() {
  std::vector<IPlayerCommand *> activeCommands;

  for (auto &pair : keyBindings_) {
    int key = pair.first;

    for (auto &binding : pair.second) {
      bool active = binding.isPressed ? IsKeyPressed(key) : IsKeyReleased(key);
      if (active) {
        activeCommands.push_back(binding.command.get());
      }
    }
  }

  return activeCommands;
}
