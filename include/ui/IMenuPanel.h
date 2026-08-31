#pragma once
#include "raylib.h"
#include <functional>

class IMenuPanel {
public:
    virtual ~IMenuPanel() = default;

    // Core lifecycle
    virtual void Update(float dt) = 0;
    virtual void Render() const = 0;
    
    // Input handling
    virtual void HandleInput(Vector2 mousePos, bool mousePressed, bool mouseReleased) = 0;

    // Optional: Entry and Exit transitions
    virtual void TriggerEntry() {}
    virtual void TriggerExit() {}
    
    // Optional: Setting a callback when the panel wants to close itself
    virtual void SetOnCloseCallback(std::function<void()> onClose) {}
};
