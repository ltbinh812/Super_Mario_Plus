#pragma once 
#include "StateManager.h"
#include "raylib.h"
#include <algorithm>

class Game {
    private:
    const float fixedDt = 1.0f / 60.0f;
    float accumulator = 0.0f;
    StateManager stateManager;

    public: 
    void runGame();
     
};