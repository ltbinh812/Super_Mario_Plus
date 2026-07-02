#include "Game.h"
#include "AssetManager.h"

void Game::runGame() {

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        dt = std::min(dt, 0.25f);
        accumulator+= dt;

        stateManager.HandleInput();
        stateManager.Process();

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