#pragma once
#include "BaseLevelState.h"
#include "ButtonGroup.h"
#include "CharacterInfoPanel.h"
#include "Buttons.h"
#include "UIScaler.h"
#include "raylib.h"
#include <vector>
#include <map>
#include <memory>
#include "ITransition.h"

class MainMenuState : public BaseLevelState {
private:
    Texture2D titleTexture;
    Texture2D vignetteTex;
    Texture2D hudFrameTexture;
    Texture2D menuPanelTex;
    Texture2D panelTex2;
    Texture2D line0Tex;
    Texture2D line1Tex;
    
    Font customFont;

    // Bộ quy đổi từ khung thiết kế ảo 1280x720 sang màn hình thật. Mọi hằng số
    // pixel bên dưới đều là toạ độ TRONG KHUNG THIẾT KẾ, phải đi qua ui_ trước
    // khi dùng để vẽ. Nhờ vậy bố cục giữ nguyên tỉ lệ trên mọi độ phân giải.
    UIScaler ui_;

    // UI Layout state (giá trị ĐÃ quy đổi sang pixel màn hình thật)
    Vector2 panelPos;
    float panelScale;
    float titleScale;
    float btnScale;
    
    // Animation state for title
    Vector2 titleTargetPos;
    Vector2 titleStartPos;
    float titleCurrentY;
    float titleAnimTime;
    
    std::map<std::string, std::shared_ptr<IMenuPanel>> menuPanels;
    std::string activeGroup;
    
    // Shared arrow textures
    Texture2D arrowLeft, arrowRight, arrowLeftPress, arrowRightPress;
    // Global animation timer
    float elapsedTime;
    
    // Bot AI Timers
    float p1BotTimer;
    float p2BotTimer;
    
    // Top-left standalone Game Mode Toggle
    
    // Lerp helper
    float EaseOutBack(float t) const;
    void SimulateBotInput(Player* player, float& botTimer, float dt);

    std::unique_ptr<ITransition> transitionOut;
    std::unique_ptr<ITransition> transitionIn;
    bool isChangingState;
    bool isTransitioningIn;
    bool isPvPModeSelected = false;

public:
    MainMenuState();
    ~MainMenuState() override;
    
    void HandleInput() override;
    void Process() override;
    void Update(float dt) override;
    void Render(float alpha) const override;
};
