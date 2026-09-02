#include "Game.h"
#include "AssetManager.h"
#include "MainMenuState.h"
#include "LoadingState.h"
#include "infrastructure/AudioManager.h"

Game::Game() {
    // Tải các âm thanh (SFX) toàn cục
    AssetManager::getInstance().loadSound("coin_sound", "assets/sfx/coin_sound.wav");
    AssetManager::getInstance().loadSound("buff_use_sound", "assets/sfx/buff_use_sound.wav");
    AssetManager::getInstance().loadSound("chest_open_sound", "assets/sfx/chest_open_sound.wav");
    AssetManager::getInstance().loadSound("door_open_sound", "assets/sfx/door_open_sound.wav");
    AssetManager::getInstance().loadSound("pickup_sound", "assets/sfx/pickup_sound.wav");
    AssetManager::getInstance().loadSound("swim_sound", "assets/sfx/swim_sound.wav");
    AssetManager::getInstance().loadSound("hang_sound", "assets/sfx/hang_sound.mp3");
    AssetManager::getInstance().loadSound("typewriter_sound", "assets/sfx/typewriter_sound.mp3");
    AssetManager::getInstance().loadSound("click_sound", "assets/sfx/click_sound.wav");
    AssetManager::getInstance().loadSound("boom_explosion_sound", "assets/sfx/boom_explosion_sound.wav");
    AssetManager::getInstance().loadSound("poison_explosion_sound", "assets/sfx/poison_explosion_sound.wav");

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