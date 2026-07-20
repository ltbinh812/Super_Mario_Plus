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
    AssetManager::getInstance().loadTexture("Goku_run", "assets/Goku_animation/run.png");
    AssetManager::getInstance().loadTexture("Goku_idle", "assets/Goku_animation/idle.png");
    AssetManager::getInstance().loadTexture("Goku_jump", "assets/Goku_animation/jump.png");
    AssetManager::getInstance().loadTexture("Goku_fall", "assets/Goku_animation/fall.png");
    AssetManager::getInstance().loadTexture("Goku_dash", "assets/Goku_animation/dash.png");
    AssetManager::getInstance().loadTexture("Goku_punch1", "assets/Goku_animation/punch1.png");



    

    // Chạy logic game chính 
    Game game;
    game.runGame();

    // Đóng cửa sổ và giải phóng bộ nhớ
    AssetManager::getInstance().clearAll();
    CloseWindow();
    return 0;
}
