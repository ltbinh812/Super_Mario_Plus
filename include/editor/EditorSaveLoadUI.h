#pragma once
#include <functional>
#include <string>
#include <vector>
#include "raylib.h"

// Giao diện (Overlay) hiển thị 10 slot để Save/Load map
enum class SaveLoadMode { Save, Load };

class EditorSaveLoadUI {
public:
    EditorSaveLoadUI();
    void init(); // Load texture UI if needed

    // Draw the overlay
    void render(float screenW, float screenH, SaveLoadMode mode) const;

    // Handle mouse input and return action
    void handleInput(float screenW, float screenH,
                     std::function<void(int slot)> onSlotSelected,
                     std::function<void()> onCancel);

private:
    std::vector<std::string> slotStatus_; // Caches "Slot X (Empty/Used)" strings
    void refreshSlotStatus(); // Đọc trạng thái từ thư mục saves/

    float cachedTimer_ = 0.0f; // Limit disk check frequency
};
