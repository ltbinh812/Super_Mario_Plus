#include "IngameSettingsPanel.h"
#include "SettingsManager.h"
#include <iostream>
#include <cmath>
#include <algorithm>

IngameSettingsPanel::IngameSettingsPanel() {
    cogwheelTex_ = LoadTexture("assets/UI_screens/cogwheel.png");
    panelTex2_ = LoadTexture("assets/UI_screens/menu_panel_sub.png");
    arrowLeft_ = LoadTexture("assets/UI_screens/menu_icon_arrow_left.png");
    arrowLeftPress_ = LoadTexture("assets/UI_screens/menu_icon_arrow_left_press.png");
    arrowRight_ = LoadTexture("assets/UI_screens/menu_icon_arrow_right.png");
    arrowRightPress_ = LoadTexture("assets/UI_screens/menu_icon_arrow_right_press.png");
    line1Tex_ = LoadTexture("assets/UI_screens/menu_deco_line_1.png");

    settingsGroup_ = std::make_shared<ButtonGroup>();
}

IngameSettingsPanel::~IngameSettingsPanel() {
    if (cogwheelTex_.id != 0) UnloadTexture(cogwheelTex_);
    if (panelTex2_.id != 0) UnloadTexture(panelTex2_);
    if (arrowLeft_.id != 0) UnloadTexture(arrowLeft_);
    if (arrowLeftPress_.id != 0) UnloadTexture(arrowLeftPress_);
    if (arrowRight_.id != 0) UnloadTexture(arrowRight_);
    if (arrowRightPress_.id != 0) UnloadTexture(arrowRightPress_);
    if (line1Tex_.id != 0) UnloadTexture(line1Tex_);
}

void IngameSettingsPanel::init(float screenWidth, float screenHeight, std::function<void()> onQuitToMenu) {
    onQuitToMenuCallback_ = onQuitToMenu;

    float panelScale = 2.0f;
    float btnScale = 2.0f;
    float gap = 20.0f * panelScale;

    float sWidth = screenWidth > 0 ? screenWidth : (float)GetScreenWidth();
    float sHeight = screenHeight > 0 ? screenHeight : (float)GetScreenHeight();
    if (sWidth <= 0) sWidth = 1280.0f;
    if (sHeight <= 0) sHeight = 720.0f;

    float settingsPanelW = panelTex2_.width * panelScale * 1.4f; // 40% wider
    float settingsPanelH = panelTex2_.height * panelScale;
    Vector2 settingsPos = { (sWidth - settingsPanelW) / 2.0f, (sHeight - settingsPanelH) / 2.0f };

    settingsGroup_ = std::make_shared<ButtonGroup>();
    settingsGroup_->SetGroupName("SETTINGS");
    settingsGroup_->SetPanel(panelTex2_, settingsPos, panelScale, settingsPanelW);
    settingsGroup_->SetArrows(arrowLeft_, arrowRight_, arrowLeftPress_, arrowRightPress_);
    settingsGroup_->SetButtonScale(btnScale);
    settingsGroup_->SetGroupName("SETTINGS", {27, 34, 54, 255}, 13.0f);
    settingsGroup_->SetHeaderButton("assets/UI_screens/undo_button_2.png", "assets/UI_screens/undo_button_2.png", [this]() {
        close();
    });

    // Add 3 Tabs
    settingsGroup_->AddTab("assets/UI_screens/bar.png", "assets/UI_screens/bar_press.png", "Controls");
    settingsGroup_->AddTab("assets/UI_screens/bar.png", "assets/UI_screens/bar_press.png", "Sounds");
    settingsGroup_->AddTab("assets/UI_screens/bar.png", "assets/UI_screens/bar_press.png", "Quit to Menu");

    // Populate P1 Keybinds
    auto& sm = SettingsManager::GetInstance();
    settingsGroup_->AddKeybind("Move Left", "Move Left", sm.GetP1Key("Move Left"), sm.GetP1DefaultKey("Move Left"), true);
    settingsGroup_->AddKeybind("Move Right", "Move Right", sm.GetP1Key("Move Right"), sm.GetP1DefaultKey("Move Right"), true);
    settingsGroup_->AddKeybind("Climb", "Climb", sm.GetP1Key("Climb"), sm.GetP1DefaultKey("Climb"), true);
    settingsGroup_->AddKeybind("Crouch", "Crouch", sm.GetP1Key("Crouch"), sm.GetP1DefaultKey("Crouch"), true);
    settingsGroup_->AddKeybind("Jump", "Jump", sm.GetP1Key("Jump"), sm.GetP1DefaultKey("Jump"), true);
    settingsGroup_->AddKeybind("Attack", "Attack", sm.GetP1Key("Attack"), sm.GetP1DefaultKey("Attack"), true);
    settingsGroup_->AddKeybind("Dash", "Dash", sm.GetP1Key("Dash"), sm.GetP1DefaultKey("Dash"), true);
    settingsGroup_->AddKeybind("Long Attack", "LongAttack", sm.GetP1Key("LongAttack"), sm.GetP1DefaultKey("LongAttack"), true);
    settingsGroup_->AddKeybind("Special Skill", "SpecialAttack", sm.GetP1Key("SpecialAttack"), sm.GetP1DefaultKey("SpecialAttack"), true);
    settingsGroup_->AddKeybind("Use Item", "Interact", sm.GetP1Key("Interact"), sm.GetP1DefaultKey("Interact"), true);
    settingsGroup_->AddKeybind("Block", "Block", sm.GetP1Key("Block"), sm.GetP1DefaultKey("Block"), true);

    // Populate P2 Keybinds
    settingsGroup_->AddKeybind("P2 Move Left", "Move Left", sm.GetP2Key("Move Left"), sm.GetP2DefaultKey("Move Left"), false);
    settingsGroup_->AddKeybind("P2 Move Right", "Move Right", sm.GetP2Key("Move Right"), sm.GetP2DefaultKey("Move Right"), false);
    settingsGroup_->AddKeybind("P2 Climb", "Climb", sm.GetP2Key("Climb"), sm.GetP2DefaultKey("Climb"), false);
    settingsGroup_->AddKeybind("P2 Crouch", "Crouch", sm.GetP2Key("Crouch"), sm.GetP2DefaultKey("Crouch"), false);
    settingsGroup_->AddKeybind("P2 Jump", "Jump", sm.GetP2Key("Jump"), sm.GetP2DefaultKey("Jump"), false);
    settingsGroup_->AddKeybind("P2 Attack", "Attack", sm.GetP2Key("Attack"), sm.GetP2DefaultKey("Attack"), false);
    settingsGroup_->AddKeybind("P2 Dash", "Dash", sm.GetP2Key("Dash"), sm.GetP2DefaultKey("Dash"), false);
    settingsGroup_->AddKeybind("P2 Long Attack", "LongAttack", sm.GetP2Key("LongAttack"), sm.GetP2DefaultKey("LongAttack"), false);
    settingsGroup_->AddKeybind("P2 Special Skill", "SpecialAttack", sm.GetP2Key("SpecialAttack"), sm.GetP2DefaultKey("SpecialAttack"), false);
    settingsGroup_->AddKeybind("P2 Use Item", "Interact", sm.GetP2Key("Interact"), sm.GetP2DefaultKey("Interact"), false);
    settingsGroup_->AddKeybind("P2 Block", "Block", sm.GetP2Key("Block"), sm.GetP2DefaultKey("Block"), false);

    // Populate Sounds
    settingsGroup_->AddSlider("Master Volume", 
        []() { return SettingsManager::GetInstance().GetMasterVolume(); },
        [](float v) { SettingsManager::GetInstance().SetMasterVolume(v); }
    );
    settingsGroup_->AddSlider("Music Volume", 
        []() { return SettingsManager::GetInstance().GetMusicVolume(); },
        [](float v) { SettingsManager::GetInstance().SetMusicVolume(v); }
    );
    settingsGroup_->AddSlider("Map Background Sound", 
        []() { return SettingsManager::GetInstance().GetBackgroundSoundVolume(); },
        [](float v) { SettingsManager::GetInstance().SetBackgroundSoundVolume(v); }
    );
    settingsGroup_->AddSlider("Player Sounds", 
        []() { return SettingsManager::GetInstance().GetPlayerSFXVolume(); },
        [](float v) { SettingsManager::GetInstance().SetPlayerSFXVolume(v); }
    );
    settingsGroup_->AddSlider("Enemy Sounds", 
        []() { return SettingsManager::GetInstance().GetEnemySFXVolume(); },
        [](float v) { SettingsManager::GetInstance().SetEnemySFXVolume(v); }
    );

    // Setup Quit to Menu callback
    settingsGroup_->SetOnQuitToMenu([this]() {
        if (onQuitToMenuCallback_) {
            onQuitToMenuCallback_();
        }
    });

    settingsGroup_->UpdateLayout(settingsPos.y + 50.0f * panelScale, gap);

    // Decorate Settings Group
    float sLineScaleX = panelScale * (settingsPanelW / (panelTex2_.width * panelScale));
    float sLine1X = settingsPos.x + (settingsPanelW - line1Tex_.width * sLineScaleX) / 2.0f;
    float sLine1Y = settingsPos.y + 28.0f * panelScale;
    settingsGroup_->AddDecoLine(line1Tex_, {sLine1X, sLine1Y}, {sLineScaleX, panelScale});
}

bool IngameSettingsPanel::handleInput(Vector2 mousePos, bool mousePressed, bool mouseReleased) {
    if (IsKeyPressed(KEY_ESCAPE)) {
        toggle();
        return true;
    }

    if (isOpen_) {
        // Forward input to settings panel
        settingsGroup_->HandleInput(mousePos, mousePressed, mouseReleased);

        // Click outside panel to close
        if (mousePressed && !settingsGroup_->ContainsPoint(mousePos)) {
            close();
        }
        return true; // Consumes input when open
    } else {
        // Check cogwheel button click
        isCogwheelHovered_ = CheckCollisionPointRec(mousePos, cogwheelBounds_);
        if (isCogwheelHovered_ && mousePressed) {
            open();
            return true;
        }
    }

    return false;
}

void IngameSettingsPanel::process() {
    // Process step for any frame-by-frame business logic
}

void IngameSettingsPanel::update(float dt) {
    if (isOpen_) {
        settingsGroup_->Update(dt);
    } else {
        // Cogwheel hover animation
        float targetScale = isCogwheelHovered_ ? 1.15f : 1.0f;
        cogwheelAnimScale_ += (targetScale - cogwheelAnimScale_) * 10.0f * dt;
        
        if (isCogwheelHovered_) {
            cogwheelAngle_ += 60.0f * dt;
            if (cogwheelAngle_ >= 360.0f) cogwheelAngle_ -= 360.0f;
        }
    }
}

void IngameSettingsPanel::render(float alpha) const {
    if (isOpen_) {
        // Dark translucent backdrop
        int sW = GetScreenWidth();
        int sH = GetScreenHeight();
        DrawRectangle(0, 0, sW, sH, {0, 0, 0, 160});

        // Render Settings Group
        settingsGroup_->Render();
    } else {
        // Render Cogwheel Icon in top-left corner
        if (cogwheelTex_.id != 0) {
            float size = 36.0f * cogwheelAnimScale_;
            float cx = cogwheelBounds_.x + cogwheelBounds_.width / 2.0f;
            float cy = cogwheelBounds_.y + cogwheelBounds_.height / 2.0f;
            
            Rectangle src = {0.0f, 0.0f, (float)cogwheelTex_.width, (float)cogwheelTex_.height};
            Rectangle dest = {cx, cy, size, size};
            Vector2 origin = {size / 2.0f, size / 2.0f};
            
            Color tint = isCogwheelHovered_ ? WHITE : Color{240, 240, 240, 220};
            DrawTexturePro(cogwheelTex_, src, dest, origin, cogwheelAngle_, tint);
        }
    }
}

void IngameSettingsPanel::open() {
    isOpen_ = true;
    settingsGroup_->ResetActiveTab();
    settingsGroup_->TriggerEntry();
}

void IngameSettingsPanel::close() {
    isOpen_ = false;
}

void IngameSettingsPanel::toggle() {
    if (isOpen_) close();
    else open();
}
