#pragma once 
// #include <memory>
// class GameState;

// enum class CommandType {
//     None, Push, Pop, Change
// };

// struct StateCommand {
//     CommandType type = CommandType::None;
//     std::unique_ptr<GameState> state;
// };


class StateManager;

class IGameCommand {
public:
    virtual ~IGameCommand() = default;
    
    virtual void Execute(StateManager& manager) = 0; 
};