#include "UIComponent.h"

Button::Button() {

}

Button& Button::setPosition(Vector2 position) {
    position_ = position;
    updateBound();
    return *this;
}

Button& Button::setSize(Vector2 size) {
    size_ = size;
    updateBound();
    return *this;
}

Button& Button::setLabel(const std::string& label) {
    label_ = label;
    return *this;
}

Button& Button::setRadius(float radius) {
    borderRadius_ = radius;
    return *this;
}

Button& Button::setOnClick(std::function<void()> onClick) {
    onClick_ = onClick;
    return *this;
}
void Button::updateBound() {
    bound_ = {position_.x, position_.y, size_.x, size_.y};
}

void Button::handleInput(Vector2 mousePos, bool mousePressed, bool mouseReleased) {
    if (isEnabled_ == false) return;


    isHovered_ = CheckCollisionPointRec(mousePos, bound_);

    if (mousePressed == true) {
        if (isHovered_) {
            isPressed_ = true;
        }
    }

    if (mouseReleased == true) {
        if (isHovered_ && isPressed_) {
            isClicked_ = true;
        }

        isPressed_ = false;
    }
    
}

void Button::update() {
    if (isClicked_ == true) {
        isClicked_ = false;
        onClick_();
    } 
}

void Button::render() const {
    Color color = RAYWHITE; // Default

    if (!isEnabled_) color = GRAY;
    else if (isPressed_) color = DARKGRAY;
    else if (isHovered_) color = LIGHTGRAY;

    // Calculate roundness (0.0 to 1.0) based on the shortest side
    float minSide = (bound_.width < bound_.height) ? bound_.width : bound_.height;
    float roundness = 0.0f;
    if (minSide > 0) {
        // Multiply by 2 because radius applies to both sides of the dimension
        roundness = (borderRadius_ * 2.0f) / minSide; 
        
        // Clamp to prevent visual glitching if radius is set too high
        if (roundness > 1.0f) roundness = 1.0f; 
    }

    int segments = 16; // Adjust for smoothness

    // Draw button background
    DrawRectangleRounded(bound_, roundness, segments, color);

    // Draw border
    // Note: DrawRectangleRoundedLines takes line thickness
    DrawRectangleRoundedLines(bound_, roundness, segments, 2.0f, BLACK);

    // Center text
    int fontSize = 20;
    int textWidth = MeasureText(label_.c_str(), fontSize);
    int textX = static_cast<int>(bound_.x + (bound_.width - textWidth) / 2.0f);
    int textY = static_cast<int>(bound_.y + (bound_.height - fontSize) / 2.0f);

    DrawText(label_.c_str(), textX, textY, fontSize, BLACK);
}