#pragma once

#include "GameState.h"
#include "MapCamera.h"
#include "InputHandler.h"

#include "Player.h"
#include "TileMap.h"

#include <memory>
#include <string>

class World03State : public GameState {
public:
    World03State();
    ~World03State() override;

    void HandleInput() override;
    void Process() override;
    void Update(float dt) override;
    void Render(float alpha) const override;


private:
    std::unique_ptr<Player> player1;
    std::unique_ptr<Player> player2;
    InputHandler player1Handler;
    InputHandler player2Handler;


    TileMap map;
    MapCamera mapCamera;
    std::string currentLevel;

    void TransitionToLevel(const std::string &nextLevel, const std::string &dir, float triggerGlobalX, float triggerGlobalY);
};
