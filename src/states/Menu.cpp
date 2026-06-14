#include "Menu.h"
#include "GameManager.h"
#include "World.h"
#include "raylib.h"
#include <memory>

MenuState::MenuState(GameManager* gameManager) : gameManager_(gameManager) {}

void MenuState::HandleInput() {
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        gameManager_->ChangeState(std::make_unique<World1_1State>(gameManager_));
    }
}

void MenuState::Update(float dt) {
    // Menu không cần logic update liên tục lúc này
}

void MenuState::Draw() {
    ClearBackground({ 40, 44, 52, 255 }); // Dark background

    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    const char* title = "SUPER MARIO PLUS";
    int titleWidth = MeasureText(title, 60);
    DrawText(title, screenW / 2 - titleWidth / 2, screenH / 3, 60, YELLOW);

    const char* prompt = "Press ENTER to Start";
    int promptWidth = MeasureText(prompt, 30);
    
    // Blinking effect
    if (((int)(GetTime() * 2)) % 2 == 0) {
        DrawText(prompt, screenW / 2 - promptWidth / 2, screenH / 2 + 50, 30, RAYWHITE);
    }
}
