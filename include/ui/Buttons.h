#pragma once 
#include "raylib.h"
#include <queue>
#include <string>
#include <functional>
class Button {
    private: 
    bool isHovered_     = false;
    bool isPressed_     = false;
    bool isClicked_     = false;
    bool isEnabled_     = true;
    Vector2 position_ = {0.0f,0.0f};
    Vector2 size_ = {100.0f, 50.0f};
    std::string label_ = "default";
    float borderRadius_ = 5.0f;
    Rectangle bound_ = {position_.x, position_.y, size_.x, size_.y};
    
    std::function<void()> onClick_ = []() {};

    public: 
    Button();
    Button& setPosition(Vector2 position);
    Button& setSize(Vector2 size);
    Button& setLabel(const std::string& label);
    Button& setRadius(float radius);
    Button& setOnClick(std::function<void()> onClick);
    void updateBound();

    void handleInput(Vector2 mousePos, bool mousePressed, bool mouseReleased);
    void update();
    void render() const;
};