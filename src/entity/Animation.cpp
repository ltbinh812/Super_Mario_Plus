#include "Animation.h"

Animation::Animation(const Texture2D& tex, int frameNum, float frameTime, float scale)
        : texture_(&tex),
          frameNum(frameNum),
          frameTime(frameTime),
          curFrame(0),
          timer(0.0f),
          loop_(true),
          scale_(scale)
    {
        frameWidth  = tex.width / frameNum;
        frameHeight = tex.height;
    }


void Animation::update(float dt) {
    timer += dt;

    while (timer >= frameTime) {
        timer -= frameTime;
        if (loop_) {
            curFrame = (curFrame + 1) % frameNum;
        } else {
            if (curFrame < frameNum - 1) {
                curFrame++;
            } else {
                timer = 0.0f;
                break;
            }
        }
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