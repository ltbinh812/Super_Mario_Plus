#include "SettingsOverlay.h"
#include "Menu.h"
#include <memory>

SettingsOverlay::SettingsOverlay(GameManager* gameManager) : gameManager_(gameManager) {
}

void SettingsOverlay::HandleInput() {
    if (IsKeyPressed(KEY_ESCAPE)) {
        isFinished_ = true;
    }

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mousePos = GetMousePosition();
        Rectangle menuBtn = { (float)GetScreenWidth()/2 - 100, (float)GetScreenHeight()/2 + 40, 200, 50 };
        if (CheckCollisionPointRec(mousePos, menuBtn)) {
            // N+¶t Menu quay vﬂ+¸ Menu State
            gameManager_->ChangeState(std::make_unique<MenuState>(gameManager_));
        }
    }
}

void SettingsOverlay::Update(float dt) {
    // Kh+¶ng c+¶ logic -Êﬂ+÷ng, t-¨nh
}

void SettingsOverlay::Draw() {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.7f));
    DrawText("SETTINGS (Press ESC to resume)", GetScreenWidth()/2 - 200, 100, 20, WHITE);
    
    Rectangle saveBtn = { (float)GetScreenWidth()/2 - 100, (float)GetScreenHeight()/2 - 80, 200, 50 };
    Rectangle loadBtn = { (float)GetScreenWidth()/2 - 100, (float)GetScreenHeight()/2 - 20, 200, 50 };
    Rectangle menuBtn = { (float)GetScreenWidth()/2 - 100, (float)GetScreenHeight()/2 + 40, 200, 50 };

    DrawRectangleRec(saveBtn, GRAY);
    DrawText("SAVE (WIP)", saveBtn.x + 40, saveBtn.y + 15, 20, BLACK);

    DrawRectangleRec(loadBtn, GRAY);
    DrawText("LOAD (WIP)", loadBtn.x + 40, loadBtn.y + 15, 20, BLACK);

    DrawRectangleRec(menuBtn, LIGHTGRAY);
    DrawText("RETURN TO MENU", menuBtn.x + 10, menuBtn.y + 15, 20, BLACK);
}
