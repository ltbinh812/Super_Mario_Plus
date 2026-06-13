#pragma once 
#include <memory>
class GameState;

enum class CommandType {
    None, Push, Pop, Change
};

struct Command {
    CommandType type = CommandType::None;
    std::unique_ptr<GameState> state;
};