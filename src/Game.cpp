#include "Game.h"

void Game::runGame() {
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        dt = std::min(dt, 0.25f);
        accumulator+= dt;

        stateManager.handleInput();

        while(accumulator >= fixedDt) {
            accumulator -= fixedDt;
            stateManager.update(fixedDt);
        }

        float alpha = accumulator / fixedDt;

        BeginDrawing();
            ClearBackground(DARKGRAY);
        stateManager.render(alpha);
        EndDrawing();

    }
}