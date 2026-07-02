#include "Animation.h"

Animation::Animation(const Texture2D& tex, int frameNum, float frameTime)
        : texture_(&tex),
          frameNum(frameNum),
          frameTime(frameTime),
          curFrame(0),
          timer(0.0f)
    {
        frameWidth  = tex.width / frameNum;
        frameHeight = tex.height;
    }


void Animation::update(float dt) {
    timer += dt;

    while (timer >= frameTime) {
        timer -= frameTime;
        curFrame = (curFrame + 1) % frameNum;
    }
}

Rectangle Animation::getCurrentFrame() const {
    Rectangle rec;
    rec.y = 0;
    rec.x = curFrame * frameWidth;
    rec.width = frameWidth;
    rec.height = frameHeight;
    return rec;
}

const Texture2D& Animation::getTexture() const {
    return *texture_;
}

void Animation::resetAnimation() {
    curFrame = 0;
    timer = 0.0f;
}