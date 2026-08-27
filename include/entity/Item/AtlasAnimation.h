#pragma once
#include "raylib.h"
#include <string>

// Manages animations for items based on a single Atlas rectangle.
// It mathematically slices the master rectangle into frames.
class AtlasAnimation {
public:
    AtlasAnimation();
    AtlasAnimation(const std::string& baseFrameName, int frameCount, float frameTime, bool loop = true);

    void update(float dt);
    void reset();

    // Returns the sliced rectangle for the current frame
    Rectangle getCurrentSourceRect() const;
    
    // Returns the texture containing the atlas
    const Texture2D& getTexture() const;

    bool isValid() const { return frameCount_ > 0 && !baseFrameName_.empty(); }
    bool isFinished() const { return !loop_ && curFrame_ >= frameCount_ - 1; }
    bool isLooping() const { return loop_; }

private:
    std::string baseFrameName_;
    int frameCount_;
    float frameTime_;
    float timer_;
    int curFrame_;
    bool loop_;
};
