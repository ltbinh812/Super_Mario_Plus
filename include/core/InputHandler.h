#pragma once
#include "IEntityCommand.h"
#include "raylib.h"
#include <memory>
#include <unordered_map>
#include <vector>

struct KeyBinding {
  std::unique_ptr<IEntityCommand> command;
  bool isContinuous;
};

class InputHandler {
private:
  std::unordered_map<int, KeyBinding> keyBindings_;

public:
  InputHandler() = default;
  void clearBindings();
  void bindKey(int key, std::unique_ptr<IEntityCommand> command, bool isContinuous);
  std::vector<IEntityCommand *> handleInput();
};

class Entity;

struct PlayerController {
  InputHandler handler;
  Entity* target;
};
