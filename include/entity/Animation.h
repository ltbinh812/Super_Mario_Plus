#pragma once 
#include "raylib.h"

class Animation {
private:
    const Texture2D* texture_;
    int frameNum;
    int curFrame;
    float frameTime;
    float timer;
    bool loop_;

    int frameWidth;
    int frameHeight;

public:
    Animation(const Texture2D& tex, int frameNum, float frameTime);
    void update(float dt);
    Rectangle getCurrentFrame() const;
    const Texture2D& getTexture() const;
    int getFrameNum() const { return frameNum; }
    float getFrameTime() const { return frameTime; }
    void setLoop(bool loop) { loop_ = loop; }
    bool isLooping() const { return loop_; }

    void resetAnimation();
};