#pragma once 

class StateManager;

class IGameCommand {
public:
    virtual ~IGameCommand() = default;
    
    virtual void Execute(StateManager& manager) = 0; 
};