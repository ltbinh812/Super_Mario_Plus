#ifndef WORLD_H
#define WORLD_H

#include "GameState.h"
#include "Player.h"
#include "Platform.h"
#include "Cloud.h"
#include "Decoration.h"
#include "CameraManager.h"
#include <vector>

class GameManager; // Forward declaration

class World1_1State : public GameState {
public:
    World1_1State(GameManager* gameManager);
    ~World1_1State() override = default;

    void HandleInput() override;
    void Update(float dt) override;
    void Draw() override;

private:
    GameManager* gameManager_;
    
    float worldWidth_;
    float groundY_;

    Player player_;
    CameraManager camera_;
    std::vector<Platform> platforms_;
    std::vector<Cloud> clouds_;
    std::vector<Decoration> decorations_;
};

#endif
