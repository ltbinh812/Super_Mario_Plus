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
#include "infrastructure/AudioManager.h"

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
      
    // Danh sách nhạc nền Menu (bạn có thể thêm nhiều bài ở đây)
    std::vector<std::string> menuPlaylist = {
        "assets/audio/see_you_in_heaven.mp3",
        // "assets/audio/menu_music_2.mp3", // Ví dụ bài thứ 2
        // "assets/audio/menu_music_3.mp3"  // Ví dụ bài thứ 3
    };
    
    // Kích hoạt chế độ Playlist
    AudioManager::getInstance().PlayMusicPlaylist(menuPlaylist);
      
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
    
    // =========================================================================
    // BỐ CỤC ĐỘC LẬP ĐỘ PHÂN GIẢI
    //
    // Toàn bộ menu được canh trong khung thiết kế ảo 1280x720 (xem UIScaler.h).
    // ui_ đã tự đọc kích thước màn hình thật trong constructor của nó; ở đây
    // gọi Refresh() một lần nữa cho chắc, vì MainMenuState được dựng sau khi
    // main.cpp gọi MaximizeWindow() nên kích thước có thể vừa mới đổi.
    //
    // QUY TẮC: mọi con số pixel viết trong file này là toạ độ TRONG KHUNG
    // THIẾT KẾ. Dùng ui_.S() cho độ dài/hệ số phóng, ui_.X()/Y()/Pos() cho
    // toạ độ. Không được viết thẳng pixel vào biến layout.
    // =========================================================================
    ui_.Refresh();

    // Ba hệ số phóng ảnh gốc — con số canh mắt ở 1280x720, nhân thêm ui_.Factor()
    // để ra đúng tỉ lệ đó trên mọi màn hình.
    panelScale = ui_.S(3.3f);
    titleScale = ui_.S(1.35f);
    btnScale   = ui_.S(2.45f);

    // Đẩy panel sang phải để tránh mấy cái chậu vuông trên khung HUD
    const float paddingLeftDesign = 220.0f;

    // Calculate base metrics from actual texture size
    float panelW = menuPanelTex.width * panelScale;
    float panelH = menuPanelTex.height * panelScale;
    float titleW = titleTexture.width * titleScale;
    float titleH = titleTexture.height * titleScale;

    // X lấy theo khung thiết kế (có cộng lề letterbox), Y căn giữa khung thiết kế.
    panelPos = { ui_.X(paddingLeftDesign), ui_.CenterY() - panelH / 2.0f };

    // Title is aligned center inside the top part of the panel
    titleTargetPos = { panelPos.x + panelW / 2.0f - titleW / 2.0f, panelPos.y + 20.0f * panelScale };
    titleStartPos = { titleTargetPos.x, ui_.S(-400.0f) }; // Start way above screen
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
            this->PushStateCommand(std::make_unique<::PopStateCommand>());
        }, baseDelay + 4 * delayIncrement);

    // Calculate available space for buttons inside panel
    float remainingSpace = panelH - (titleTargetPos.y - panelPos.y + titleH) - ui_.S(20.0f);
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
    
    // Offset Settings panel from main panel and make it 40% wider than original.
    // Căn giữa theo KHUNG THIẾT KẾ (ui_.CenterX/CenterY) chứ không phải giữa màn
    // hình thật — trên màn siêu rộng hai cách này khác nhau, và căn theo khung
    // mới giữ đúng tương quan với panel chính.
    Vector2 panelPos = { ui_.CenterX() - menuPanelTex.width * panelScale / 2.0f,
                         ui_.CenterY() - menuPanelTex.height * panelScale / 2.0f };

    float settingsPanelW = panelTex2.width * panelScale * 1.4f; // 40% wider
    Vector2 settingsPos = { panelPos.x + ui_.S(30.0f), panelPos.y + ui_.S(30.0f) };
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
    settingsGroup->AddTab("assets/UI_screens/bar.png", "assets/UI_screens/bar_press.png", "Display");
    
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
    settingsGroup->AddSlider("Music Volume", 
        []() { return SettingsManager::GetInstance().GetMusicVolume(); },
        [](float v) { SettingsManager::GetInstance().SetMusicVolume(v); }
    );
    settingsGroup->AddSlider("Map Background Sound", 
        []() { return SettingsManager::GetInstance().GetBackgroundSoundVolume(); },
        [](float v) { SettingsManager::GetInstance().SetBackgroundSoundVolume(v); }
    );
    settingsGroup->AddSlider("Player Sounds", 
        []() { return SettingsManager::GetInstance().GetPlayerSFXVolume(); },
        [](float v) { SettingsManager::GetInstance().SetPlayerSFXVolume(v); }
    );
    settingsGroup->AddSlider("Enemy Sounds", 
        []() { return SettingsManager::GetInstance().GetEnemySFXVolume(); },
        [](float v) { SettingsManager::GetInstance().SetEnemySFXVolume(v); }
    );
    
    // Cùng công tắc rung màn hình như bảng settings lúc chơi — chung một giá trị
    // trong SettingsManager nên hai nơi không thể lệch nhau.
    settingsGroup->AddToggle("Screen Shake",
        []() { return SettingsManager::GetInstance().IsScreenShakeEnabled(); },
        [](bool v) { SettingsManager::GetInstance().SetScreenShakeEnabled(v); }
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
    
    // Generate Vignette Texture.
    // Vignette được kéo giãn phủ kín màn hình khi vẽ, nên sinh đúng bằng kích
    // thước màn hình thật (không đi qua ui_) để không bị mờ do phóng lại.
    int vW = GetScreenWidth()  > 0 ? GetScreenWidth()  : (int)UIScaler::kDesignWidth;
    int vH = GetScreenHeight() > 0 ? GetScreenHeight() : (int)UIScaler::kDesignHeight;
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
    if (IsKeyPressed(KEY_ESCAPE) && activeGroup != "Main") {
        activeGroup = "Main";
        return;
    }

    // Only handle UI clicks. Do NOT call BaseLevelState::HandleInput() so manual player control is disabled.
    Vector2 mousePos = GetMousePosition();
    bool mousePressed = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    bool mouseReleased = IsMouseButtonReleased(MOUSE_LEFT_BUTTON);
    
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
