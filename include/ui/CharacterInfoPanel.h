#pragma once
#include "IMenuPanel.h"
#include "UIAtlasAnimator.h"
#include <vector>
#include <string>
#include <memory>

enum class CharInfoTransition {
    SLIDING_UP,
    HOVERING,
    ENTERING,
    IDLE,
    EXITING,
    SLIDING_DOWN
};

class CharacterInfoPanel : public IMenuPanel {
private:
    std::vector<std::string> characters = {
        "Goku", "Naruto", "Luffy", "Kakashi", "Sasuke", "Zoro"
    };
    int selectedIndex = 0;
    
    std::unique_ptr<UIAtlasAnimator> appearAnim_;
    std::unique_ptr<UIAtlasAnimator> disappearAnim_;
    CharInfoTransition transitionState_;
    
    float introAnimTime_ = 0.0f;
    float hoverTime_ = 0.0f;
    float enterTime_ = 0.0f;
    float exitTime_ = 0.0f;
    float scale_ = 2.6f; // Adjusted scale as requested
    
    std::function<void()> onCloseCallback_;

public:
    CharacterInfoPanel();
    ~CharacterInfoPanel() override;

    void Update(float dt) override;
    void Render() const override;
    void HandleInput(Vector2 mousePos, bool mousePressed, bool mouseReleased) override;

    void TriggerEntry() override;
    void SetOnCloseCallback(std::function<void()> onClose) override {
        onCloseCallback_ = onClose;
    }
};
