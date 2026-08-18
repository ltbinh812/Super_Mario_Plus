#pragma once
#include "IPlayerCommand.h"
#include "raylib.h"
#include <memory>
#include <unordered_map>
#include <vector>

struct KeyBinding {
  std::unique_ptr<IPlayerCommand> command;
  int triggerMode; // 0: Released, 1: Down, 2: Pressed
};

class InputHandler {
private:
  std::unordered_map<int, std::vector<KeyBinding>> keyBindings_;

public:
  InputHandler() = default;
  void clearBindings();
  void bindKey(int key, std::unique_ptr<IPlayerCommand> command, int triggerMode);
  std::vector<IPlayerCommand *> handleInput();
};

class Player;

struct PlayerController {
  InputHandler handler;
  Player* target;
};
