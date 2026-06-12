#pragma once 
#include "raylib.h"
#include "Command.h"
#include <queue>
#include <string>
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
    
    Command command_ = Command{CommandType::Null};

    public: 
    Button(Command command);
    Button& setPosition(Vector2 position);
    Button& setSize(Vector2 size);
    Button& setLabel(const std::string& label);
    Button& setRadius(float radius);
    void updateBound();

    void handleInput(Vector2 mousePos, bool mousePressed, bool mouseReleased);
    void update(std::queue<Command> &commandQueue);
    void render() const;
};