#ifndef OVERLAYUI_H
#define OVERLAYUI_H

class OverlayUI {
public:
    virtual ~OverlayUI() = default;
    
    // Handle input / click
    virtual void HandleInput() = 0;
    
    // Update UI logic
    virtual void Update(float dt) = 0;
    
    // Draw UI to screen
    virtual void Draw() = 0;
    
    // Check if UI is finished
    virtual bool IsFinished() const = 0;
};

#endif
