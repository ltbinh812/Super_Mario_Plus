#include "Game.h"
#include "AssetManager.h"
#include "raylib.h"

int main() {
    // Khởi tạo các cờ cửa sổ (có thể điều chỉnh kích thước, VSync)
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT);
    
    // Mở cửa sổ
    InitWindow(1280, 720, "Super Mario Plus");
    SetTargetFPS(60);

    //load các Asset
    AssetManager::getInstance().loadTexture("mario_run", "assets/Run.png");
    AssetManager::getInstance().loadTexture("mario_idle", "assets/Idle.png");
    AssetManager::getInstance().loadTexture("mario_jump", "assets/Jump.png");
    

    // Chạy logic game chính 
    Game game;
    game.runGame();

    // Đóng cửa sổ và giải phóng bộ nhớ
    AssetManager::getInstance().clearAll();
    CloseWindow();
    return 0;
}
