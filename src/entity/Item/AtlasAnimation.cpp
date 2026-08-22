#include "AtlasAnimation.h"
#include "ItemAtlasRegistry.h"

AtlasAnimation::AtlasAnimation() 
    : baseFrameName_(""), frameCount_(0), frameTime_(0.0f), timer_(0.0f), curFrame_(0), loop_(false) {}

AtlasAnimation::AtlasAnimation(const std::string& baseFrameName, int frameCount, float frameTime, bool loop)
    : baseFrameName_(baseFrameName), frameCount_(frameCount), frameTime_(frameTime), 
      timer_(0.0f), curFrame_(0), loop_(loop) {}

void AtlasAnimation::update(float dt) {
    if (frameCount_ <= 1) return;
    
    timer_ += dt;
    while (timer_ >= frameTime_) {
        timer_ -= frameTime_;
        if (loop_) {
            curFrame_ = (curFrame_ + 1) % frameCount_;
        } else {
            if (curFrame_ < frameCount_ - 1) {
                curFrame_++;
            } else {
                timer_ = 0.0f; // Stop at last frame
                break;
            }
        }
    }
}

void AtlasAnimation::reset() {
    curFrame_ = 0;
    timer_ = 0.0f;
}

Rectangle AtlasAnimation::getCurrentSourceRect() const {
    auto& reg = ItemAtlasRegistry::getInstance();
    Rectangle masterRect = reg.getFrame(baseFrameName_);
    
    Rectangle frameRect = masterRect;
    if (frameCount_ > 1) {
        float frameWidth = masterRect.width / frameCount_;
        frameRect.x += curFrame_ * frameWidth;
        frameRect.width = frameWidth;
    }
    
    // Prevent texture bleeding
    frameRect.x += 0.1f;
    frameRect.y += 0.1f;
    frameRect.width -= 0.2f;
    frameRect.height -= 0.2f;
    
    return frameRect;
}

const Texture2D& AtlasAnimation::getTexture() const {
    auto& reg = ItemAtlasRegistry::getInstance();
    return reg.getTexture(baseFrameName_);
}
