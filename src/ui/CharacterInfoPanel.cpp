#include "CharacterInfoPanel.h"
#include <iostream>
#include <cmath>

CharacterInfoPanel::CharacterInfoPanel() {
    appearAnim_ = std::make_unique<UIAtlasAnimator>("assets/UI_screens/page_appear-0.png", "assets/UI_screens/page_appear.json", 20.0f);
    disappearAnim_ = std::make_unique<UIAtlasAnimator>("assets/UI_screens/page_disappear-0.png", "assets/UI_screens/page_disappear.json", 24.0f);
    
    transitionState_ = CharInfoTransition::SLIDING_UP;
}

CharacterInfoPanel::~CharacterInfoPanel() = default;

void CharacterInfoPanel::TriggerEntry() {
    transitionState_ = CharInfoTransition::SLIDING_UP;
    introAnimTime_ = 0.0f;
    hoverTime_ = 0.0f;
    enterTime_ = 0.0f;
    exitTime_ = 0.0f;
    appearAnim_->Reset();
    disappearAnim_->Reset();
}

void CharacterInfoPanel::HandleInput(Vector2 mousePos, bool mousePressed, bool mouseReleased) {
    if (transitionState_ != CharInfoTransition::IDLE) {
        return; // lock input during transitions
    }
    
    // Check click outside
    if (mousePressed) {
        int screenWidth = GetScreenWidth();
        int screenHeight = GetScreenHeight();
        float scrollWidth = 476.0f * scale_;
        float scrollHeight = 375.0f * scale_;
        Rectangle scrollRect = {
            screenWidth / 2.0f - scrollWidth / 2.0f,
            screenHeight / 2.0f - scrollHeight / 2.0f,
            scrollWidth,
            scrollHeight
        };
        
        if (!CheckCollisionPointRec(mousePos, scrollRect)) {
            transitionState_ = CharInfoTransition::EXITING;
            exitTime_ = 0.0f;
            disappearAnim_->Reset();
            return;
        }
    }
    
    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE)) {
        transitionState_ = CharInfoTransition::EXITING;
        exitTime_ = 0.0f;
        disappearAnim_->Reset();
        return;
    }

    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        selectedIndex++;
        if (selectedIndex >= characters.size()) {
            selectedIndex = 0;
        }
    }
    
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        selectedIndex--;
        if (selectedIndex < 0) {
            selectedIndex = characters.size() - 1;
        }
    }

    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_J)) {
        std::string selectedChar = characters[selectedIndex];
        std::cout << "[CharacterInfoPanel] Viewing info for: " << selectedChar << "\n";
    }
}

void CharacterInfoPanel::Update(float dt) {
    if (transitionState_ == CharInfoTransition::SLIDING_UP) {
        introAnimTime_ += dt * 1.5f; // Faster slide up so bounce completes quicker
        if (introAnimTime_ >= 1.0f) {
            introAnimTime_ = 1.0f;
            transitionState_ = CharInfoTransition::HOVERING;
            hoverTime_ = 0.0f;
        }
    } else if (transitionState_ == CharInfoTransition::HOVERING) {
        hoverTime_ += dt;
        if (hoverTime_ >= 0.8f) { // Hover for 0.8 seconds before opening
            transitionState_ = CharInfoTransition::ENTERING;
            appearAnim_->Reset(); // Use custom frame logic in render instead of play
            enterTime_ = 0.0f;
        }
    } else if (transitionState_ == CharInfoTransition::ENTERING) {
        enterTime_ += dt * 3.5f; // Open much faster!
        if (enterTime_ >= 1.0f) {
            enterTime_ = 1.0f;
            transitionState_ = CharInfoTransition::IDLE;
        }
    } else if (transitionState_ == CharInfoTransition::EXITING) {
        exitTime_ += dt * 3.5f; // Fast close
        if (exitTime_ >= 1.0f) {
            exitTime_ = 1.0f;
            transitionState_ = CharInfoTransition::SLIDING_DOWN;
            introAnimTime_ = 0.0f; // Reuse introAnimTime_ for drop
        }
    } else if (transitionState_ == CharInfoTransition::SLIDING_DOWN) {
        introAnimTime_ += dt * 1.5f; // Drop speed
        if (introAnimTime_ >= 1.0f) {
            introAnimTime_ = 1.0f;
            if (onCloseCallback_) {
                onCloseCallback_();
            }
        }
    }
}

void CharacterInfoPanel::Render() const {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    Vector2 targetCenter = { screenWidth / 2.0f, screenHeight / 2.0f };
    Vector2 currentCenter = targetCenter;
    float currentScale = scale_;

    if (transitionState_ == CharInfoTransition::SLIDING_UP) {
        // Elastic ease-out (wider amplitude, fewer bounces)
        float t = introAnimTime_;
        float easeOut = 1.0f;
        if (t > 0.0f && t < 1.0f) {
            float p = 0.7f; // Increased period = fewer bounces
            // Less decay (-6.0f instead of -10.0f) = wider amplitude
            easeOut = pow(2.0f, -6.0f * t) * sin((t - p / 4.0f) * (2.0f * PI) / p) + 1.0f;
        }
        float startY = screenHeight + 300.0f;
        currentCenter.y = startY + (targetCenter.y - startY) * easeOut;
        currentScale = scale_ * (0.6f + 0.4f * easeOut); // Scale up while bouncing
        appearAnim_->RenderFrame(0, currentCenter, currentScale);
    } 
    else if (transitionState_ == CharInfoTransition::HOVERING) {
        // Sine wave hover effect
        currentCenter.y += sin(hoverTime_ * 5.0f) * 10.0f;
        appearAnim_->RenderFrame(0, currentCenter, scale_);
    }
    else if (transitionState_ == CharInfoTransition::ENTERING) {
        // Quadratic ease-out for frame index
        // Starts fast (skipping static frames quickly) and slows down during the unrolling
        float t = enterTime_;
        float ease = 1.0f - (1.0f - t) * (1.0f - t);
        
        int totalFrames = appearAnim_->GetTotalFrames();
        int frameIndex = (int)(ease * (totalFrames - 1));
        if (frameIndex < 0) frameIndex = 0;
        if (frameIndex >= totalFrames) frameIndex = totalFrames - 1;
        
        appearAnim_->RenderFrame(frameIndex, currentCenter, scale_);
    } 
    else if (transitionState_ == CharInfoTransition::EXITING) {
        float t = exitTime_;
        float ease = t * t; // Quadratic ease in
        int totalFrames = disappearAnim_->GetTotalFrames();
        int frameIndex = (int)(ease * (totalFrames - 1));
        if (frameIndex < 0) frameIndex = 0;
        if (frameIndex >= totalFrames) frameIndex = totalFrames - 1;
        
        disappearAnim_->RenderFrame(frameIndex, currentCenter, scale_);
    } 
    else if (transitionState_ == CharInfoTransition::SLIDING_DOWN) {
        // Back ease-in (go up slightly then drop down)
        float t = introAnimTime_;
        float s = 1.70158f;
        float easeIn = t * t * ((s + 1.0f) * t - s); 
        
        float startY = screenHeight / 2.0f;
        float endY = screenHeight + 300.0f;
        
        currentCenter.y = startY + (endY - startY) * easeIn;
        disappearAnim_->RenderFrame(disappearAnim_->GetTotalFrames() - 1, currentCenter, scale_);
    }
    else if (transitionState_ == CharInfoTransition::IDLE) {
        appearAnim_->RenderFrame(appearAnim_->GetTotalFrames() - 1, currentCenter, scale_);
        
        // Draw Text inside the scroll (Adjusted for scale_)
        const char* title = "CHARACTER INFO";
        int titleFontSize = (int)(30 * scale_ / 1.5f);
        int titleWidth = MeasureText(title, titleFontSize);
        DrawText(title, screenWidth / 2 - titleWidth / 2, screenHeight / 2 - (int)(140 * scale_ / 1.5f), titleFontSize, {80, 50, 20, 255}); // Dark brown ink

        int startY = screenHeight / 2 - (int)(90 * scale_ / 1.5f);
        int spacing = (int)(40 * scale_ / 1.5f);

        for (int i = 0; i < characters.size(); i++) {
            Color color = (i == selectedIndex) ? RED : BLACK;
            std::string text = characters[i];
            if (i == selectedIndex) {
                text = "> " + text + " <";
            }

            int fontSize = (i == selectedIndex) ? (int)(25 * scale_ / 1.5f) : (int)(20 * scale_ / 1.5f);
            int textWidth = MeasureText(text.c_str(), fontSize);
            
            DrawText(text.c_str(), screenWidth / 2 - textWidth / 2, startY + i * spacing, fontSize, color);
        }

        const char* instructions = "ESC: Back  |  UP/DOWN: Navigate";
        int instFontSize = (int)(20 * scale_ / 1.5f);
        int instWidth = MeasureText(instructions, instFontSize);
        DrawText(instructions, screenWidth / 2 - instWidth / 2, screenHeight / 2 + (int)(150 * scale_ / 1.5f), instFontSize, DARKGRAY);
    }
}
