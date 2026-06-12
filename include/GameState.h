#pragma once 
#include "Command.h"
#include <queue>
class GameState {
    public:
    virtual ~GameState() = default;
    virtual void handleInput() = 0;
    virtual void update(float dt) = 0;
    virtual Command processCommand() = 0;
    virtual void render(float alpha) const = 0;


};

