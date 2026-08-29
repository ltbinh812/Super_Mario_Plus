#include "EditorSaveLoadUI.h"
#include "CustomMapSerializer.h"
#include <iostream>

EditorSaveLoadUI::EditorSaveLoadUI() {
    slotStatus_.resize(10, "Slot (Unknown)");
}

void EditorSaveLoadUI::init() {
    refreshSlotStatus();
}

void EditorSaveLoadUI::refreshSlotStatus() {
    for (int i = 0; i < 10; ++i) {
        if (CustomMapSerializer::slotExists(i)) {
            slotStatus_[i] = "Slot " + std::to_string(i) + " (Overwrite)";
        } else {
            slotStatus_[i] = "Slot " + std::to_string(i) + " (Empty)";
        }
    }
}

void EditorSaveLoadUI::render(float screenW, float screenH, bool isSaveMode) const {
    // Darken background
    DrawRectangle(0, 0, (int)screenW, (int)screenH, Color{0, 0, 0, 150});

    // Modal background
    float panelW = 400.0f;
    float panelH = 500.0f;
    float panelX = (screenW - panelW) / 2.0f;
    float panelY = (screenH - panelH) / 2.0f;

    DrawRectangleRec({panelX, panelY, panelW, panelH}, Color{30, 30, 45, 240});
    DrawRectangleLinesEx({panelX, panelY, panelW, panelH}, 2.0f, Color{100, 100, 150, 255});

    // Title
    const char* title = isSaveMode ? "SAVE MAP" : "LOAD MAP";
    int titleW = MeasureText(title, 20);
    DrawText(title, (int)(panelX + (panelW - titleW) / 2), (int)(panelY + 20), 20, WHITE);

    // Slots
    float btnW = panelW - 60.0f;
    float btnH = 32.0f;
    float startY = panelY + 70.0f;
    
    for (int i = 0; i < 10; ++i) {
        float by = startY + i * (btnH + 8.0f);
        Rectangle btnRect = { panelX + 30.0f, by, btnW, btnH };
        
        bool isHovered = CheckCollisionPointRec(GetMousePosition(), btnRect);
        Color bg = isHovered ? Color{70, 130, 180, 255} : Color{50, 50, 70, 255};
        
        // Highlight empty slots differently when loading (can't load empty)
        bool exists = (slotStatus_[i].find("Empty") == std::string::npos);
        if (!isSaveMode && !exists) {
            bg = Color{40, 40, 50, 255}; // Dim if loading and empty
        }

        DrawRectangleRec(btnRect, bg);
        DrawRectangleLinesEx(btnRect, 1.0f, Color{100, 100, 140, 255});

        int textW = MeasureText(slotStatus_[i].c_str(), 16);
        Color tc = (!isSaveMode && !exists) ? Color{120, 120, 130, 255} : WHITE;
        DrawText(slotStatus_[i].c_str(), (int)(btnRect.x + 20), (int)(btnRect.y + 8), 16, tc);
    }

    // Cancel Button
    Rectangle cancelRect = { panelX + panelW/2 - 50.0f, panelY + panelH - 40.0f, 100.0f, 30.0f };
    bool cancelHover = CheckCollisionPointRec(GetMousePosition(), cancelRect);
    DrawRectangleRec(cancelRect, cancelHover ? Color{200, 80, 80, 255} : Color{150, 50, 50, 255});
    DrawRectangleLinesEx(cancelRect, 1.0f, Color{250, 100, 100, 255});
    DrawText("CANCEL", (int)(cancelRect.x + 22), (int)(cancelRect.y + 8), 16, WHITE);
}

void EditorSaveLoadUI::handleInput(float screenW, float screenH,
                                   std::function<void(int slot)> onSlotSelected,
                                   std::function<void()> onCancel) {
    // Limit refresh rate to avoid spamming disk
    cachedTimer_ += GetFrameTime();
    if (cachedTimer_ > 1.0f) {
        refreshSlotStatus();
        cachedTimer_ = 0.0f;
    }

    if (!IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) return;

    Vector2 mp = GetMousePosition();
    float panelW = 400.0f;
    float panelH = 500.0f;
    float panelX = (screenW - panelW) / 2.0f;
    float panelY = (screenH - panelH) / 2.0f;

    // Check Cancel
    Rectangle cancelRect = { panelX + panelW/2 - 50.0f, panelY + panelH - 40.0f, 100.0f, 30.0f };
    if (CheckCollisionPointRec(mp, cancelRect)) {
        if (onCancel) onCancel();
        return;
    }

    // Check Slots
    float btnW = panelW - 60.0f;
    float btnH = 32.0f;
    float startY = panelY + 70.0f;
    
    for (int i = 0; i < 10; ++i) {
        float by = startY + i * (btnH + 8.0f);
        Rectangle btnRect = { panelX + 30.0f, by, btnW, btnH };
        if (CheckCollisionPointRec(mp, btnRect)) {
            if (onSlotSelected) onSlotSelected(i);
            return;
        }
    }
}
