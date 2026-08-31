#pragma once
#include <string>
#include <vector>
#include "raylib.h"

struct AtlasFrame {
    Rectangle frame;       // x, y, w, h in the sprite sheet
    Rectangle spriteSourceSize; // x, y, w, h inside the original image
    Vector2 sourceSize;    // original width and height
};

class UIAtlasAnimator {
private:
    Texture2D texture_;
    std::vector<AtlasFrame> frames_;
    int currentFrame_;
    float frameTime_;
    float timer_;
    bool isPlaying_;
    bool isFinished_;

public:
    UIAtlasAnimator(const std::string& texturePath, const std::string& jsonPath, float fps = 24.0f);
    ~UIAtlasAnimator();

    void Play();
    void Stop();
    void Reset();
    void Update(float dt);
    void Render(Vector2 position, float scale = 1.0f) const;

    bool IsFinished() const { return isFinished_; }
    bool IsPlaying() const { return isPlaying_; }
    int GetCurrentFrameIndex() const { return currentFrame_; }
    int GetTotalFrames() const { return frames_.size(); }
    const AtlasFrame& GetCurrentFrame() const { return frames_[currentFrame_]; }
    
    // Sometimes we want to render a specific frame (e.g. the last frame when idle)
    void RenderFrame(int frameIndex, Vector2 position, float scale = 1.0f) const;
};
