#include "Game.h"
#include "AssetManager.h"
#include "raylib.h"
#include <ctime>
#include <cstdlib>

int main() {
    srand(time(nullptr));
    // Khởi tạo các cờ cửa sổ (có thể điều chỉnh kích thước, VSync)
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT);
    
    // Mở cửa sổ
    InitWindow(1280, 720, "Super Mario Plus");
    MaximizeWindow(); // Phóng to cửa sổ tối đa (vừa toàn màn hình nhưng không ngập lấp Taskbar)
    SetTargetFPS(60);

    //load các Asset
    AssetManager::getInstance().loadTexture("Goku_run", "assets/Goku_animation/run.png");
    AssetManager::getInstance().loadTexture("Goku_idle", "assets/Goku_animation/idle.png");
    AssetManager::getInstance().loadTexture("Goku_jump", "assets/Goku_animation/jump.png");
    AssetManager::getInstance().loadTexture("Goku_fall", "assets/Goku_animation/fall.png");
    AssetManager::getInstance().loadTexture("Goku_dash", "assets/Goku_animation/dash.png");
    AssetManager::getInstance().loadTexture("Goku_crouch", "assets/Goku_animation/crouch.png");
    AssetManager::getInstance().loadTexture("Goku_punch1", "assets/Goku_animation/punch1.png");
    AssetManager::getInstance().loadTexture("Goku_block", "assets/Goku_animation/block.png");
    AssetManager::getInstance().loadTexture("Goku_hurt", "assets/Goku_animation/hurt.png");
    AssetManager::getInstance().loadTexture("Goku_die", "assets/Goku_animation/die.png");
    AssetManager::getInstance().loadTexture("Goku_fireball", "assets/Goku_animation/bullet.png");
    AssetManager::getInstance().loadTexture("Goku_long_attack", "assets/Goku_animation/longattack2.png");



    AssetManager::getInstance().loadTexture("Luffy_run", "assets/Luffy_animation/run.png");
    AssetManager::getInstance().loadTexture("Luffy_idle", "assets/Luffy_animation/idle.png");
    AssetManager::getInstance().loadTexture("Luffy_jump", "assets/Luffy_animation/jump.png");
    AssetManager::getInstance().loadTexture("Luffy_fall", "assets/Luffy_animation/fall.png");
    AssetManager::getInstance().loadTexture("Luffy_dash", "assets/Luffy_animation/dash.png");
    AssetManager::getInstance().loadTexture("Luffy_punch1", "assets/Luffy_animation/punch1.png");


    

    // Chạy logic game chính 
    Game game;
    game.runGame();

    // Đóng cửa sổ và giải phóng bộ nhớ
    AssetManager::getInstance().clearAll();
    CloseWindow();
    return 0;
}
