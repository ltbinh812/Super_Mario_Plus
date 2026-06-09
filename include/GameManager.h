#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "GameState.h"
#include <memory>

class GameManager {
public:
    GameManager();
    void ChangeState(std::unique_ptr<GameState> newState);
    
    void HandleInput();
    void Update(float dt);
    void Draw();

private:
    std::unique_ptr<GameState> currentState_;
};

#endif
