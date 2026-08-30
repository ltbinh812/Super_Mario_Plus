#pragma once
#include "ButtonGroup.h"
#include "raylib.h"
#include <functional>
#include <memory>

// =============================================================================
// IngameSettingsPanel — Giao diện Settings in-game cho 6 World & Custom Map.
//
// Tuân thủ GEMINI.md: 1 Class / 1 File, 4 giai đoạn Game Loop.
// Chứa nút bánh răng cưa (cogwheel) ở góc trái trên và panel Settings (Controls,
// Sounds, Quit to Menu) sao chép nguyên bản từ Main Menu.
// =============================================================================
class IngameSettingsPanel {
private:
    std::shared_ptr<ButtonGroup> settingsGroup_;
    
    // Cogwheel Button
    Texture2D cogwheelTex_ = {0};
    Rectangle cogwheelBounds_ = {25.0f, 25.0f, 38.0f, 38.0f};
    bool isCogwheelHovered_ = false;
    float cogwheelAnimScale_ = 1.0f;
    float cogwheelAngle_ = 0.0f;
    
    // Panel textures
    Texture2D panelTex2_ = {0};
    Texture2D arrowLeft_ = {0};
    Texture2D arrowLeftPress_ = {0};
    Texture2D arrowRight_ = {0};
    Texture2D arrowRightPress_ = {0};
    Texture2D line1Tex_ = {0};
    
    bool isOpen_ = false;
    std::function<void()> onQuitToMenuCallback_;

public:
    IngameSettingsPanel();
    ~IngameSettingsPanel();

    void init(float screenWidth, float screenHeight, std::function<void()> onQuitToMenu);

    bool handleInput(Vector2 mousePos, bool mousePressed, bool mouseReleased);
    void process();
    void update(float dt);
    void render(float alpha = 1.0f) const;

    bool isOpen() const { return isOpen_; }
    void open();
    void close();
    void toggle();
};
