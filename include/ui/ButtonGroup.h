#pragma once
#include "Buttons.h"
#include "raylib.h"
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include "IMenuPanel.h"

struct MenuButtonState {
    Button btn;
    Texture2D texNormal;
    Texture2D texPress;
    Vector2 targetPos;
    Vector2 startPos;
    float currentY;
    float animationTime;
    float delay;
    bool isVisible;
    std::string action;
    std::string labelText; // For runtime text rendering
};

struct KeybindRow {
    std::string label;
    std::string actionName;
    int currentKey;
    int defaultKey;
    MenuButtonState keyBtn;
    MenuButtonState resetBtn;
    bool isP1;
    bool isListening;
};

struct SliderRow {
    std::string label;
    std::function<float()> getter;
    std::function<void(float)> setter;
    Texture2D barTex;
    Rectangle barRect;
    Rectangle knobRect;
    bool isDragging;
};


class ButtonGroup : public IMenuPanel {
private:
    std::vector<MenuButtonState> buttons_;
    Texture2D panelTex_;
    Vector2 panelPos_;
    float panelScale_;
    float panelW_;
    float panelH_;
    float btnScale_;
    
    // Arrow Assets
    Texture2D arrowLeft_;
    Texture2D arrowRight_;
    Texture2D arrowLeftPress_;
    Texture2D arrowRightPress_;
    
    // Arrow State
    float arrowAnimTime_;
    int hoveredIndex_;
    float currentArrowAlpha_;
    float currentArrowOffset_;
    
    // Title inside group (optional)
    Texture2D titleTex_;
    Vector2 titlePos_;
    float titleScale_;
    
    struct DecoLine {
        Texture2D tex;
        Vector2 pos;
        Vector2 scale;
    };
    std::vector<DecoLine> decoLines_;
    
    bool isEntering_ = false;
    float introAnimTime_ = 1.0f;
    
    std::string groupName_;
    Color groupNameColor_;
    float groupNameSize_;
    
    // Header Button
    bool hasHeaderBtn_;
    MenuButtonState headerBtn_;
    float headerBtnCurrentScale_ = 1.0f;
    
    // Tab System
    std::vector<MenuButtonState> tabs_;
    std::string activeTab_ = "Controls";
    
    // Settings UI
    std::vector<KeybindRow> keybinds_;
    Texture2D barTex_;
    Texture2D barPressTex_;
    Texture2D resetBtnTex_;
    Texture2D resetBtnPressTex_;
    
    // Sliders
    std::vector<SliderRow> sliders_;
    
    // Scrolling
    float scrollY_ = 0.0f;
    float maxScrollY_ = 0.0f;
    Rectangle scissorArea_ = {0,0,0,0};
    
    // Custom Font
    Font customFont_;
    bool hasCustomFont_ = false;
    
    // Key listening
    bool isListeningForKey_ = false;
    KeybindRow* listeningRow_ = nullptr;
    
    // Helper to load cached textures for keybind rows
    void LoadSettingsTextures();
    
public:
    ButtonGroup();
    ~ButtonGroup();
    
    void SetPanel(Texture2D panelTex, Vector2 pos, float scale, float overrideWidth = 0.0f) {
        panelTex_ = panelTex;
        panelPos_ = pos;
        panelScale_ = scale;
        panelW_ = overrideWidth > 0 ? overrideWidth : panelTex.width * scale;
        panelH_ = panelTex.height * scale;
    }
    void SetArrows(Texture2D al, Texture2D ar, Texture2D alp, Texture2D arp);
    void SetButtonScale(float scale);
    void SetTitle(Texture2D title, Vector2 pos, float scale);
    void SetGroupName(const std::string& name, Color color = LIGHTGRAY, float fontSize = 20.0f);
    void SetHeaderButton(const std::string& normalPath, const std::string& pressPath, std::function<void()> onClick);
    void AddDecoLine(Texture2D tex, Vector2 pos, Vector2 scale = {1.0f, 1.0f});
    
    bool ContainsPoint(Vector2 point) const;
    
    void AddButton(const std::string& normalPath, const std::string& pressPath, const std::string& action, std::function<void()> onClick, float delay, const std::string& labelText = "");
    void AddTab(const std::string& normalPath, const std::string& pressPath, const std::string& tabName);
    void AddKeybind(const std::string& label, const std::string& actionName, int currentKey, int defaultKey, bool isP1);
    void AddSlider(const std::string& label, std::function<float()> getter, std::function<void(float)> setter);

    
    void SetCustomFont(Font font) { customFont_ = font; hasCustomFont_ = true; }
    void UpdateLayout(float startY, float gap);
    
    void Update(float dt) override;
    void Render() const override;
    void HandleInput(Vector2 mousePos, bool mousePressed, bool mouseReleased) override;
    
    bool IsEntering() const { return isEntering_; }
    void TriggerEntry() override;
    
    float GetPanelWidth() const { return panelTex_.width * panelScale_; }
    float GetPanelHeight() const { return panelTex_.height * panelScale_; }
    float GetPanelX() const { return panelPos_.x; }
    float GetPanelY() const { return panelPos_.y; }
};
