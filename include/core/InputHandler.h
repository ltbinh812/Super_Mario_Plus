#pragma once
#include "IPlayerCommand.h"
#include "raylib.h"
#include <memory>
#include <unordered_map>
#include <vector>

enum class InputType {
  PRESSED,
  DOWN,
  RELEASED
};

struct KeyBinding {
  std::unique_ptr<IPlayerCommand> command;
  InputType type;
};

class InputHandler {
private:
  std::unordered_map<int, std::vector<KeyBinding>> keyBindings_;

public:
  InputHandler() = default;
  void clearBindings();
  void bindKey(int key, std::unique_ptr<IPlayerCommand> command, InputType type);
  std::vector<IPlayerCommand *> handleInput();
};

class Player;

struct PlayerController {
  InputHandler handler;
  Player* target;
};
