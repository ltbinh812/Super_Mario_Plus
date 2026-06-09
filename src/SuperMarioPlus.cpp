#include "SuperMarioPlus.h"
#include "GameManager.h"
#include "Menu.h"
#include "raylib.h"
#include <memory>

void RunGame() {
    GameManager game;
    // Khởi tạo trạng thái ban đầu là Menu
    game.ChangeState(std::make_unique<MenuState>(&game));

    // ---- Main Game Loop ----
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        // 1. Nhận phím dựa theo State hiện tại
        game.HandleInput();

        // 2. Cập nhật logic dựa theo State hiện tại
        game.Update(dt);

        // 3. Render Đồ hoạ dựa theo State hiện tại
        BeginDrawing();
        game.Draw();
        EndDrawing();
    }
}
