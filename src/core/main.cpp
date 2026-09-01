#include "Game.h"
#include "AssetManager.h"
#include "SettingsManager.h"
#include "infrastructure/AudioManager.h"
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


    // Khởi tạo audio device
    InitAudioDevice();
    
    // Set initial volumes
    ::SetMasterVolume(SettingsManager::GetInstance().GetMasterVolume());
    AudioManager::getInstance().SetMusicVolume(SettingsManager::GetInstance().GetMusicVolume());
    AudioManager::getInstance().SetBackgroundSoundVolume(SettingsManager::GetInstance().GetBackgroundSoundVolume());

    // Chạy logic game chính 
    Game game;
    game.runGame();

    // Đóng cửa sổ và giải phóng bộ nhớ
    AssetManager::getInstance().clearAll();
    AudioManager::getInstance().clearAll();
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
