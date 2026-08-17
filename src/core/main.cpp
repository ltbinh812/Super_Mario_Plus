#include "Game.h"
#include "AssetManager.h"
#include "raylib.h"

void LoadAsset() {
        //load các Asset

    // Các player 
    // Goku

    // Luffy
    // Naruto
    // kakashi
    // sasuke
    // zoro
    // Goku - upgrade
    // Naruto - upgrafe


    // Các boss
    // Chopper 
    // Franky 
    // Itachi
    // Sasuke boss
    // Shank


    // Mobs

    

}
int main() {
    // Khởi tạo các cờ cửa sổ (có thể điều chỉnh kích thước, VSync)
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT);
    
    // Mở cửa sổ
    InitWindow(1280, 720, "Super Mario Plus");
    MaximizeWindow(); // Phóng to cửa sổ tối đa (vừa toàn màn hình nhưng không ngập lấp Taskbar)
    SetTargetFPS(60);



    

    // Chạy logic game chính 
    Game game;
    game.runGame();

    // Đóng cửa sổ và giải phóng bộ nhớ
    AssetManager::getInstance().clearAll();
    CloseWindow();
    return 0;
}
