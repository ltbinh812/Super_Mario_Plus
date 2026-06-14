#include "SettingsOverlay.h"
#include "Menu.h"
#include <memory>
#include <raylib.h>

SettingsOverlay::SettingsOverlay(std::function<void(Command&&)> pushCmd) : pushCmd_(std::move(pushCmd)) {
}

void SettingsOverlay::HandleInput() {
    if (IsKeyPressed(KEY_ESCAPE)) {
        isFinished_ = true;
    }

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mousePos = GetMousePosition();
        Rectangle menuBtn = { (float)GetScreenWidth()/2 - 100, (float)GetScreenHeight()/2 + 40, 200, 50 };
        if (CheckCollisionPointRec(mousePos, menuBtn)) {
            // Nút Menu quay về Menu State
            if (pushCmd_) {
                pushCmd_(Command{CommandType::Change, std::make_unique<MenuState>()});
            }
        }
    }
}

void SettingsOverlay::Update(float dt) {
    // Không có logic động, tĩnh
}

void SettingsOverlay::Render(float alpha) const {
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
