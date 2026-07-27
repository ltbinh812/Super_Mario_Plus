#pragma once
#include "GameState.h"
#include "TileMap.h"
#include "Player.h"
#include "InputHandler.h"
#include <memory>

class World01State : public GameState {
private:
    TileMap map;
    std::unique_ptr<Player> testPlayer;
    InputHandler playerHandler;
    float cameraX;
    float cameraY;
    float zoom;

public:
    World01State();
    ~World01State() override;

    void HandleInput() override;
    void Process() override;
    void Update(float dt) override;
    void Render(float alpha) const override;
};
