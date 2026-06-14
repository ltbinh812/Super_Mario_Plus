#include "Luigi.h"

Luigi::Luigi() {
    gravity_ = 1000.0f; // Falls slower
}

void Luigi::Draw() {
    Color green = { 0, 255, 0, 255 };
    Color blue = { 0, 0, 255, 255 };
    float bx = pos_.x;
    float by = pos_.y;
    DrawRectangle((int)bx, (int)by, (int)width_, (int)height_, green);
    DrawRectangle((int)bx, (int)(by + height_/2), (int)width_, (int)(height_/2), blue);
    if (!facingRight_) {
        DrawRectangle((int)bx, (int)(by + 4), 8, 8, BLACK);
    } else {
        DrawRectangle((int)(bx + width_ - 8), (int)(by + 4), 8, 8, BLACK);
    }
}
