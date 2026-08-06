#pragma once 
#include "raylib.h"

class Animation {
private:
    const Texture2D* texture_;
    int frameNum;
    int curFrame;
    float frameTime;
    float timer;

    int frameWidth;
    int frameHeight;

public:
    Animation(const Texture2D& tex, int frameNum, float frameTime);
    void update(float dt);
    Rectangle getCurrentFrame() const;
    const Texture2D& getTexture() const;
    int getFrameNum() const { return frameNum; }
    float getFrameTime() const { return frameTime; }

    void resetAnimation();
};