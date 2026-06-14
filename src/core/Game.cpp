#include "Game.h"

void Game::runGame() {
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        dt = std::min(dt, 0.25f);
        accumulator+= dt;

        stateManager.HandleInput();

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