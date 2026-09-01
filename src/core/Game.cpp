#include "Game.h"
#include "AssetManager.h"
#include "MainMenuState.h"
#include "LoadingState.h"
#include "infrastructure/AudioManager.h"

Game::Game() {
    auto factory = []() { return std::make_unique<MainMenuState>(); };
    stateManager.PushState(std::make_unique<LoadingState>(factory, 2.0f));
}

void Game::runGame() {

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        dt = std::min(dt, 0.25f);
        accumulator+= dt;

        stateManager.HandleInput();
        stateManager.Process();
        AudioManager::getInstance().Update();

        if (stateManager.isEmpty()) {
            break;
        }

        while(accumulator >= fixedDt) {
            accumulator -= fixedDt;
            stateManager.Update(fixedDt);
        }

        float alpha = accumulator / fixedDt;

        BeginDrawing();
            ClearBackground(DARKGRAY);
            stateManager.Render(alpha);
        EndDrawing();

    }
}