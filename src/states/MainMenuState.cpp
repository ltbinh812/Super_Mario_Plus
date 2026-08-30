#include "MainMenuState.h"
#include "StateCommands.h"
#include "SettingsManager.h"
#include "World01State.h" // For play button transition
#include "World05State.h"
#include "CharacterInfoPanel.h"
#include "MapEditorState.h" // For level editor button
#include "PlayerCommands.h"
#include "IrisTransition.h"
#include "MapSelectionState.h"
#include "LoadingState.h"

#include "raylib.h"
#include <iostream>
#include <cmath>
#include <cstdlib>

MainMenuState::MainMenuState() 
    : BaseLevelState("assets/maps/UI/menu.ldtk", "AutoLayers_advanced_demo", "Goku", "Goku"), 
      elapsedTime(0.0f), p1BotTimer(0.0f), p2BotTimer(0.0f) {
      
    isChangingState = false;
    transitionOut = std::make_unique<IrisTransition>();
    transitionIn = std::make_unique<IrisTransition>();
    transitionIn->Start(false); // Start Iris In (opening)
    isTransitioningIn = true;
      
    // Disable items parsing for Menu (user said "không quan tâm item")
    activeItems.clear();
    enableIngameSettings_ = false;

    // Load UI textures
    hudFrameTexture = LoadTexture("assets/UI_screens/menu_bg_main.png");
    titleTexture = LoadTexture("assets/UI_screens/menu_title_main.png");
    menuPanelTex = LoadTexture("assets/UI_screens/menu_panel_main.png");
    line0Tex = LoadTexture("assets/UI_screens/menu_deco_line_0.png");
    line1Tex = LoadTexture("assets/UI_screens/menu_deco_line_1.png");
    
    customFont = LoadFont("assets/config/kenney-pixel-hu.otf");
    
    // Setup Title Animation (Slide from top)
    float screenWidth = 800.0f; 
    float screenHeight = 600.0f;
    if (GetScreenWidth() > 0) {
        screenWidth = (float)GetScreenWidth();
        screenHeight = (float)GetScreenHeight();
    }
    
    panelScale = 3.3f;
    titleScale = 1.35f;
    btnScale = 2.45f;
    
    // Push the panel to the right to avoid the square pots on HUD frame
    float paddingLeft = 220.0f; 
    
    // Calculate base metrics from actual texture size
    float panelW = menuPanelTex.width * panelScale;
    float panelH = menuPanelTex.height * panelScale;
    float titleW = titleTexture.width * titleScale;
    float titleH = titleTexture.height * titleScale;
    
    // Center panel vertically
    panelPos = { paddingLeft, screenHeight / 2.0f - panelH / 2.0f };
    
    // Title is aligned center inside the top part of the panel
    titleTargetPos = { panelPos.x + panelW / 2.0f - titleW / 2.0f, panelPos.y + 20.0f * panelScale };
    titleStartPos = { titleTargetPos.x, -400.0f }; // Start way above screen
    titleAnimTime = 0.0f;
    titleCurrentY = titleStartPos.y;

    // Load Arrow Textures
    arrowLeft = LoadTexture("assets/UI_screens/menu_icon_arrow_left.png");
    arrowRight = LoadTexture("assets/UI_screens/menu_icon_arrow_right.png");
    arrowLeftPress = LoadTexture("assets/UI_screens/menu_icon_arrow_left_press.png");
    arrowRightPress = LoadTexture("assets/UI_screens/menu_icon_arrow_right_press.png");

    auto mainGroup = std::make_shared<ButtonGroup>();
    mainGroup->SetPanel(menuPanelTex, panelPos, panelScale);
    mainGroup->SetTitle(titleTexture, titleTargetPos, titleScale);
    mainGroup->SetArrows(arrowLeft, arrowRight, arrowLeftPress, arrowRightPress);
    mainGroup->SetButtonScale(btnScale);
    
    // Animation cascade delay
    float baseDelay = 0.3f;
    float delayIncrement = 0.15f;

    mainGroup->AddButton("assets/UI_screens/bar.png", "assets/UI_screens/bar_press.png", "1-PLAYER MODE",
        [this]() {
            if (!this->isChangingState) {
                this->isChangingState = true;
                this->isPvPModeSelected = false;
                this->transitionOut->Start(true);
            }
        }, baseDelay + 0 * delayIncrement);
        
    mainGroup->AddButton("assets/UI_screens/bar.png", "assets/UI_screens/bar_press.png", "2-PLAYER MODE",
        [this]() {
            if (!this->isChangingState) {
                this->isChangingState = true;
                this->isPvPModeSelected = true;
                this->transitionOut->Start(true);
            }
        }, baseDelay + 1 * delayIncrement);
        
    mainGroup->AddButton("assets/UI_screens/bar.png", "assets/UI_screens/bar_press.png", "LEVEL EDITOR",
        [this]() {
            this->PushStateCommand(std::make_unique<::PushStateCommand>(
                std::make_unique<MapEditorState>()
            ));
        }, baseDelay + 2 * delayIncrement);
        
    mainGroup->AddButton("assets/UI_screens/bar.png", "assets/UI_screens/bar_press.png", "SETTINGS",
        [this]() {
            activeGroup = "Settings";
            menuPanels["Settings"]->TriggerEntry();
        }, baseDelay + 3 * delayIncrement);
        
    mainGroup->AddButton("assets/UI_screens/bar.png", "assets/UI_screens/bar_press.png", "EXIT",
        [this]() {
            this->PushStateCommand(std::make_unique<::ChangeStateCommand>(
                std::make_unique<World05State>()
            ));
        }, baseDelay + 4 * delayIncrement);

    // Calculate available space for buttons inside panel
    float remainingSpace = panelH - (titleTargetPos.y - panelPos.y + titleH) - 20.0f;
    float btnHeight = 32.0f * btnScale;
    float totalBtnH = 5 * btnHeight;
    float gap = (remainingSpace - totalBtnH) / 6;
    float btnStartY = titleTargetPos.y + titleH + gap;

    
    mainGroup->UpdateLayout(btnStartY, gap);
    mainGroup->TriggerEntry();
    
    // Decorate Main Group
    float titleY = panelPos.y + 20.0f * panelScale;
    float line0X = panelPos.x + (panelW - line0Tex.width * panelScale) / 2.0f;
    float line0Y = panelPos.y + 0.5f * panelScale; // push up, close to top edge
    mainGroup->AddDecoLine(line0Tex, {line0X, line0Y}, {panelScale, panelScale});
    
    float line1X = panelPos.x + (panelW - line1Tex.width * panelScale) / 2.0f;
    float line1Y = panelPos.y + menuPanelTex.height * panelScale - 25.0f * panelScale; // push down, close to bottom edge
    mainGroup->AddDecoLine(line1Tex, {line1X, line1Y}, {panelScale, panelScale});
    
    menuPanels["Main"] = mainGroup;
    
    // Settings Sub-Group
    panelTex2 = LoadTexture("assets/UI_screens/menu_panel_sub.png");
    auto settingsGroup = std::make_shared<ButtonGroup>();
    settingsGroup->SetGroupName("SETTINGS");
    
    // Offset Settings panel from main panel and make it 40% wider than original
    float sWidth = screenWidth > 0 ? screenWidth : 800.0f;
    float sHeight = screenHeight > 0 ? screenHeight : 600.0f;
    Vector2 panelPos = { (sWidth - menuPanelTex.width * panelScale) / 2.0f, (sHeight - menuPanelTex.height * panelScale) / 2.0f };
    
    float settingsPanelW = panelTex2.width * panelScale * 1.4f; // 40% wider
    Vector2 settingsPos = { panelPos.x + 30.0f, panelPos.y + 30.0f };
    settingsGroup->SetPanel(panelTex2, settingsPos, panelScale, settingsPanelW);
    settingsGroup->SetArrows(arrowLeft, arrowRight, arrowLeftPress, arrowRightPress);
    settingsGroup->SetButtonScale(btnScale);
    
    settingsGroup->SetGroupName("SETTINGS", {27, 34, 54, 255}, 13.0f); // smaller text
    settingsGroup->SetHeaderButton("assets/UI_screens/undo_button_2.png", "assets/UI_screens/undo_button_2.png", [this]() {
        activeGroup = "Main";
    });
    
    
    // Add Tabs
    settingsGroup->AddTab("assets/UI_screens/bar.png", "assets/UI_screens/bar_press.png", "Controls");
    settingsGroup->AddTab("assets/UI_screens/bar.png", "assets/UI_screens/bar_press.png", "Sounds");
    
    // Populate P1 Keybinds
    settingsGroup->AddKeybind("Move Left", "Move Left", SettingsManager::GetInstance().GetP1Key("Move Left"), SettingsManager::GetInstance().GetP1DefaultKey("Move Left"), true);
    settingsGroup->AddKeybind("Move Right", "Move Right", SettingsManager::GetInstance().GetP1Key("Move Right"), SettingsManager::GetInstance().GetP1DefaultKey("Move Right"), true);
    settingsGroup->AddKeybind("Climb", "Climb", SettingsManager::GetInstance().GetP1Key("Climb"), SettingsManager::GetInstance().GetP1DefaultKey("Climb"), true);
    settingsGroup->AddKeybind("Crouch", "Crouch", SettingsManager::GetInstance().GetP1Key("Crouch"), SettingsManager::GetInstance().GetP1DefaultKey("Crouch"), true);
    settingsGroup->AddKeybind("Jump", "Jump", SettingsManager::GetInstance().GetP1Key("Jump"), SettingsManager::GetInstance().GetP1DefaultKey("Jump"), true);
    settingsGroup->AddKeybind("Attack", "Attack", SettingsManager::GetInstance().GetP1Key("Attack"), SettingsManager::GetInstance().GetP1DefaultKey("Attack"), true);
    settingsGroup->AddKeybind("Dash", "Dash", SettingsManager::GetInstance().GetP1Key("Dash"), SettingsManager::GetInstance().GetP1DefaultKey("Dash"), true);
    settingsGroup->AddKeybind("Long Attack", "LongAttack", SettingsManager::GetInstance().GetP1Key("LongAttack"), SettingsManager::GetInstance().GetP1DefaultKey("LongAttack"), true);
    settingsGroup->AddKeybind("Special Skill", "SpecialAttack", SettingsManager::GetInstance().GetP1Key("SpecialAttack"), SettingsManager::GetInstance().GetP1DefaultKey("SpecialAttack"), true);
    settingsGroup->AddKeybind("Use Item", "Interact", SettingsManager::GetInstance().GetP1Key("Interact"), SettingsManager::GetInstance().GetP1DefaultKey("Interact"), true);
    settingsGroup->AddKeybind("Block", "Block", SettingsManager::GetInstance().GetP1Key("Block"), SettingsManager::GetInstance().GetP1DefaultKey("Block"), true);

    // Populate P2 Keybinds
    settingsGroup->AddKeybind("P2 Move Left", "Move Left", SettingsManager::GetInstance().GetP2Key("Move Left"), SettingsManager::GetInstance().GetP2DefaultKey("Move Left"), false);
    settingsGroup->AddKeybind("P2 Move Right", "Move Right", SettingsManager::GetInstance().GetP2Key("Move Right"), SettingsManager::GetInstance().GetP2DefaultKey("Move Right"), false);
    settingsGroup->AddKeybind("P2 Climb", "Climb", SettingsManager::GetInstance().GetP2Key("Climb"), SettingsManager::GetInstance().GetP2DefaultKey("Climb"), false);
    settingsGroup->AddKeybind("P2 Crouch", "Crouch", SettingsManager::GetInstance().GetP2Key("Crouch"), SettingsManager::GetInstance().GetP2DefaultKey("Crouch"), false);
    settingsGroup->AddKeybind("P2 Jump", "Jump", SettingsManager::GetInstance().GetP2Key("Jump"), SettingsManager::GetInstance().GetP2DefaultKey("Jump"), false);
    settingsGroup->AddKeybind("P2 Attack", "Attack", SettingsManager::GetInstance().GetP2Key("Attack"), SettingsManager::GetInstance().GetP2DefaultKey("Attack"), false);
    settingsGroup->AddKeybind("P2 Dash", "Dash", SettingsManager::GetInstance().GetP2Key("Dash"), SettingsManager::GetInstance().GetP2DefaultKey("Dash"), false);
    settingsGroup->AddKeybind("P2 Long Attack", "LongAttack", SettingsManager::GetInstance().GetP2Key("LongAttack"), SettingsManager::GetInstance().GetP2DefaultKey("LongAttack"), false);
    settingsGroup->AddKeybind("P2 Special Skill", "SpecialAttack", SettingsManager::GetInstance().GetP2Key("SpecialAttack"), SettingsManager::GetInstance().GetP2DefaultKey("SpecialAttack"), false);
    settingsGroup->AddKeybind("P2 Use Item", "Interact", SettingsManager::GetInstance().GetP2Key("Interact"), SettingsManager::GetInstance().GetP2DefaultKey("Interact"), false);
    settingsGroup->AddKeybind("P2 Block", "Block", SettingsManager::GetInstance().GetP2Key("Block"), SettingsManager::GetInstance().GetP2DefaultKey("Block"), false);
    
    // Populate Sounds
    settingsGroup->AddSlider("Master Volume", 
        []() { return SettingsManager::GetInstance().GetMasterVolume(); },
        [](float v) { SettingsManager::GetInstance().SetMasterVolume(v); }
    );
    settingsGroup->AddSlider("Background Music", 
        []() { return SettingsManager::GetInstance().GetBGMVolume(); },
        [](float v) { SettingsManager::GetInstance().SetBGMVolume(v); }
    );
    settingsGroup->AddSlider("Player Sounds", 
        []() { return SettingsManager::GetInstance().GetPlayerSFXVolume(); },
        [](float v) { SettingsManager::GetInstance().SetPlayerSFXVolume(v); }
    );
    settingsGroup->AddSlider("Enemy Sounds", 
        []() { return SettingsManager::GetInstance().GetEnemySFXVolume(); },
        [](float v) { SettingsManager::GetInstance().SetEnemySFXVolume(v); }
    );
    
    settingsGroup->UpdateLayout(settingsPos.y + 50.0f * panelScale, gap);
    
    // Decorate Settings Group
    float sLineScaleX = panelScale * (settingsPanelW / (menuPanelTex.width * panelScale));
    float sLine1X = settingsPos.x + (settingsPanelW - line1Tex.width * sLineScaleX) / 2.0f;
    float sLine1Y = settingsPos.y + 28.0f * panelScale; // right below header (title + undo button)
    settingsGroup->AddDecoLine(line1Tex, {sLine1X, sLine1Y}, {sLineScaleX, panelScale});
    
    menuPanels["Settings"] = settingsGroup;
    
    // Character Info Panel Removed
    
    activeGroup = "Main";

    // Start bots immediately
    p1BotTimer = 0.5f;
    p2BotTimer = 1.0f;
    
    // Generate Vignette Texture
    int vW = screenWidth > 0 ? (int)screenWidth : 1280;
    int vH = screenHeight > 0 ? (int)screenHeight : 720;
    Image vignetteImg = GenImageColor(vW, vH, BLANK);
    
    float radius = (float)vH / 3.0f;
    unsigned char alphaOutside = (unsigned char)(255.0f * (1.0f - 0.20f)); // 20% clear = 80% dark (204)
    unsigned char alphaCenter = (unsigned char)(255.0f * (1.0f - 0.90f));  // 90% clear = 10% dark (25)
    
    for (int y = 0; y < vH; y++) {
        for (int x = 0; x < vW; x++) {
            float dx = (float)x - vW / 2.0f;
            float dy = (float)y - vH / 2.0f;
            float dist = std::sqrt(dx*dx + dy*dy);
            
            unsigned char alpha;
            if (dist >= radius) {
                alpha = alphaOutside;
            } else {
                float factor = dist / radius; // 0 at center, 1 at edge of circle
                // We can use smooth fading
                float fade = factor * factor; // or just factor
                alpha = (unsigned char)(alphaCenter + (alphaOutside - alphaCenter) * fade);
            }
            
            Color c = { 0, 0, 0, alpha };
            ImageDrawPixel(&vignetteImg, x, y, c);
        }
    }
    vignetteTex = LoadTextureFromImage(vignetteImg);
    UnloadImage(vignetteImg);
    
    // Initialize Standalone Game Mode Toggle
    float toggleWidth = 30.0f * panelScale; // smaller
    float shiftX = 4.0f * toggleWidth; // Shift left a bit from previous 5x
    modeToggle.bound = { 30.0f + shiftX, 90.0f, toggleWidth, 14.0f * panelScale }; // shifted down to 90, height reduced
    modeToggle.animT = SettingsManager::GetInstance().IsCreativeMode() ? 1.0f : 0.0f;
}

MainMenuState::~MainMenuState() {
    UnloadTexture(titleTexture);
    UnloadTexture(vignetteTex);
    UnloadTexture(hudFrameTexture);
    UnloadTexture(arrowLeft);
    UnloadTexture(arrowLeftPress);
    UnloadTexture(arrowRight);
    UnloadTexture(arrowRightPress);
    UnloadTexture(line0Tex);
    UnloadTexture(line1Tex);
    UnloadTexture(panelTex2);
    UnloadFont(customFont);
}

float MainMenuState::EaseOutBack(float t) const {
    const float c1 = 1.70158f;
    const float c3 = c1 + 1.0f;
    return 1.0f + c3 * std::pow(t - 1.0f, 3.0f) + c1 * std::pow(t - 1.0f, 2.0f);
}

void MainMenuState::HandleInput() {
    // Only handle UI clicks. Do NOT call BaseLevelState::HandleInput() so manual player control is disabled.
    Vector2 mousePos = GetMousePosition();
    bool mousePressed = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    bool mouseReleased = IsMouseButtonReleased(MOUSE_LEFT_BUTTON);
    
    // Handle Standalone Game Mode Toggle
    modeToggle.isHovered = CheckCollisionPointRec(mousePos, modeToggle.bound);
    if (mousePressed && modeToggle.isHovered) {
        bool currentMode = SettingsManager::GetInstance().IsCreativeMode();
        SettingsManager::GetInstance().SetCreativeMode(!currentMode);
    }
    
    if (menuPanels.count("Main")) {
        menuPanels["Main"]->HandleInput(mousePos, mousePressed, mouseReleased);
    }
    
    if (activeGroup != "Main" && menuPanels.count(activeGroup)) {
        if (mousePressed) {
            // For Settings group, check click outside. 
            // For Characters group, we don't necessarily close on outside click unless handled by it.
            if (activeGroup == "Settings") {
                auto settingsGroup = std::dynamic_pointer_cast<ButtonGroup>(menuPanels[activeGroup]);
                if (settingsGroup && !settingsGroup->ContainsPoint(mousePos)) {
                    activeGroup = "Main";
                    return; // Skip input processing for settings this frame
                }
            }
        }
        menuPanels[activeGroup]->HandleInput(mousePos, mousePressed, mouseReleased);
    }
}

void MainMenuState::Process() {
    BaseLevelState::Process();
    // Prevent any map transitions in main menu
    nextLevelToLoad = "";
    
    // Force camera to stay fixed at the center of the map (overriding bot-tracking camera)
    float screenW = GetScreenWidth();
    float screenH = GetScreenHeight();
    float mapW = map.GetWidth();
    float mapH = map.GetHeight();
    
    // Calculate zoom to cover at least one dimension of the screen
    float zoomX = screenW / mapW;
    float zoomY = screenH / mapH;
    float targetZoom = std::max(zoomX, zoomY);
    if (targetZoom <= 0.0f) targetZoom = 1.0f; // safety
    
    mapCamera.Update({mapW / 2.0f, mapH / 2.0f}, {0.0f, 0.0f}, mapW, mapH, 0.0f);
    mapCamera.SetZoom(targetZoom);
}

void MainMenuState::Update(float dt) {
    if (isTransitioningIn) {
        transitionIn->Update(dt);
        if (transitionIn->IsFinished()) {
            isTransitioningIn = false;
        }
    }



    elapsedTime += dt;
    
    // Simulate bots
    if (player1) SimulateBotInput(player1.get(), p1BotTimer, dt);
    if (player2) SimulateBotInput(player2.get(), p2BotTimer, dt);
    
    // Call base physics/logic update
    BaseLevelState::Update(dt);
    
    // Update Toggle Animation
    float targetAnim = SettingsManager::GetInstance().IsCreativeMode() ? 1.0f : 0.0f;
    if (modeToggle.animT < targetAnim) {
        modeToggle.animT += dt * 8.0f;
        if (modeToggle.animT > targetAnim) modeToggle.animT = targetAnim;
    } else if (modeToggle.animT > targetAnim) {
        modeToggle.animT -= dt * 8.0f;
        if (modeToggle.animT < targetAnim) modeToggle.animT = targetAnim;
    }
    
    // Update Menu Panels
    if (menuPanels.count("Main")) {
        menuPanels["Main"]->Update(dt);
    }
    
    if (activeGroup != "Main" && menuPanels.count(activeGroup)) {
        menuPanels[activeGroup]->Update(dt);
    }

    if (isChangingState) {
        transitionOut->Update(dt);
        if (transitionOut->IsFinished()) {
            MapSelectionMode mode = isPvPModeSelected ? MapSelectionMode::PvP : MapSelectionMode::SinglePlayer;
            auto factory = [mode]() { return std::make_unique<MapSelectionState>(mode); };
            this->PushStateCommand(std::make_unique<::ChangeStateCommand>(
                std::make_unique<LoadingState>(factory, 1.0f)
            ));
        }
    }
}

void MainMenuState::Render(float alpha) const {
    ClearBackground(BLACK);
    
    // 1. Draw Map & Entities (No HUD, No Items)
    mapCamera.BeginMode();
    map.Draw();
    
    // Draw characters and enemies behind UI
    for (const auto& entity : activeEntities) {
        if (entity->getIsActive()) entity->render(alpha);
    }
    if (player1) player1->render(alpha);
    if (player2) player2->render(alpha);
    
    mapCamera.EndMode();
    
    // Draw Vignette over map
    DrawTexturePro(vignetteTex, 
        {0, 0, (float)vignetteTex.width, (float)vignetteTex.height}, 
        {0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()}, 
        {0,0}, 0.0f, WHITE);
        
    // Draw outer frame using simple Stretch (Fit to screen)
    if (hudFrameTexture.id != 0) {
        Rectangle srcRect = { 0.0f, 0.0f, (float)hudFrameTexture.width, (float)hudFrameTexture.height };
        Rectangle destRect = { 0.0f, 0.0f, (float)GetScreenWidth(), (float)GetScreenHeight() };
        DrawTexturePro(hudFrameTexture, srcRect, destRect, {0,0}, 0.0f, WHITE);
    }
    
    // Draw Groups
    if (activeGroup == "Main") {
        if (menuPanels.count("Main")) menuPanels.at("Main")->Render();
    } else {
        // Draw Main menu in background
        if (menuPanels.count("Main")) menuPanels.at("Main")->Render();
        
        // Draw dark overlay to highlight the active subgroup
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), {0, 0, 0, 150});
        
        // Draw active subgroup
        if (menuPanels.count(activeGroup)) menuPanels.at(activeGroup)->Render();
    }
    
        // Draw Standalone Game Mode Toggle (always on top)
    {
        Rectangle track = modeToggle.bound;
        
        // Measure text for dynamic plate width
        float fontSize = 11.0f * panelScale;
        std::string modeText = SettingsManager::GetInstance().IsCreativeMode() ? "CREATIVE" : "SURVIVAL";
        
        Vector2 textSize = {0, 0};
        if (customFont.texture.id != 0) {
            textSize = MeasureTextEx(customFont, modeText.c_str(), fontSize, 1.0f);
        } else {
            textSize.x = (float)MeasureText(modeText.c_str(), (int)fontSize);
            textSize.y = fontSize;
        }
        
        float textX = track.x + track.width + 12.0f;
        float totalContentWidth = (textX - track.x) + textSize.x;
        
        // Draw decorative outer border/plate behind the toggle and text
        Rectangle plate = { track.x - 12.0f, track.y - 12.0f * panelScale, 
                            totalContentWidth + 24.0f, track.height + 18.0f * panelScale };
        DrawRectangleRounded(plate, 0.4f, 16, {15, 15, 15, 220}); // Dark transparent background
        DrawRectangleRoundedLines(plate, 0.4f, 16, 2.0f, {255, 215, 0, 180}); // Soft Gold border decoration
        
        // Colors
        Color cOff = { 80, 80, 80, 255 }; // GRAY
        Color cOn = { 255, 203, 0, 255 }; // GOLD
        
        // Lerp color
        Color fillColor = {
            (unsigned char)(cOff.r + (cOn.r - cOff.r) * modeToggle.animT),
            (unsigned char)(cOff.g + (cOn.g - cOff.g) * modeToggle.animT),
            (unsigned char)(cOff.b + (cOn.b - cOff.b) * modeToggle.animT),
            255
        };
        
        // Draw track base
        DrawRectangleRounded(track, 1.0f, 16, fillColor);
        
        // Glossy highlight (glass effect on top half of track)
        Rectangle gloss = { track.x + 2.0f, track.y + 2.0f, track.width - 4.0f, track.height / 2.0f - 1.0f };
        DrawRectangleRounded(gloss, 1.0f, 16, {255, 255, 255, 40});
        
        // Inner shadow effect
        DrawRectangleRoundedLines(track, 1.0f, 16, 2.0f, {0,0,0,150});

        if (modeToggle.isHovered) {
            DrawRectangleRoundedLines(track, 1.0f, 16, 2.0f, WHITE);
        }
        
        // Draw Knob
        float knobRadius = track.height * 0.5f - 2.0f;
        float knobMinX = track.x + track.height * 0.5f;
        float knobMaxX = track.x + track.width - track.height * 0.5f;
        float knobX = knobMinX + (knobMaxX - knobMinX) * modeToggle.animT;
        float knobY = track.y + track.height / 2.0f;
        
        // Knob Drop Shadow
        DrawCircle(knobX, knobY + 2.0f, knobRadius, {0,0,0,120});
        
        // Knob Body (Gradient for 3D sphere look)
        DrawCircleGradient(knobX, knobY, knobRadius, WHITE, {200, 200, 200, 255});
        
        // Text Color matches mode
        Color textColor = SettingsManager::GetInstance().IsCreativeMode() ? GOLD : LIGHTGRAY;
        
        // Use custom font
        if (customFont.texture.id != 0) {
            DrawTextEx(customFont, modeText.c_str(), {textX, track.y + (track.height - fontSize) / 2.0f}, fontSize, 1.0f, textColor);
            DrawTextEx(customFont, "GAME MODE", {track.x, track.y - fontSize + 3.0f}, fontSize * 0.8f, 1.0f, GRAY);
        } else {
            DrawText(modeText.c_str(), (int)textX, (int)(track.y + (track.height - fontSize) / 2.0f), (int)fontSize, textColor);
            DrawText("GAME MODE", (int)track.x, (int)(track.y - fontSize + 3.0f), (int)(fontSize * 0.8f), GRAY);
        }
    }
    
    if (isTransitioningIn) {
        transitionIn->Render();
    }

    if (isChangingState) {
        transitionOut->Render();
    }
}

void MainMenuState::SimulateBotInput(Player* player, float& botTimer, float dt) {
    if (!player) return;
    
    botTimer -= dt;
    if (botTimer <= 0.0f) {
        botTimer = 5.0f + (rand() % 300) / 100.0f; // Random between 5s and 8s
        
        int action = rand() % 8; // 0-7
        
        // Stop current moves first
        StopLeftCommand().Execute(*player);
        StopRightCommand().Execute(*player);
        
        switch (action) {
            case 0: // Move Left
                MoveLeftCommand().Execute(*player);
                break;
            case 1: // Move Right
                MoveRightCommand().Execute(*player);
                break;
            case 2: // Jump
                JumpCommand().Execute(*player);
                break;
            case 3: // Attack
                AttackCommand().Execute(*player);
                break;
            case 4: // Dash
                UseSkillCommand("Dash").Execute(*player);
                break;
            case 5: // Skill 1
                UseSkillCommand("LongAttack").Execute(*player);
                break;
            case 6: // Skill 2
                UseSkillCommand("SpecialAttack").Execute(*player);
                break;
            case 7: // Block
                UseSkillCommand("Block").Execute(*player);
                break;
        }
    }
}
