#include "Mario.h"
#include "FireballAbility.h"

Mario::Mario() {
    // Stats remain default
}

void Mario::Draw() {
    Color red = { 255, 0, 0, 255 };
    Color blue = { 0, 0, 255, 255 };
    float bx = pos_.x;
    float by = pos_.y;
    DrawRectangle((int)bx, (int)by, (int)width_, (int)height_, red);
    DrawRectangle((int)bx, (int)(by + height_/2), (int)width_, (int)(height_/2), blue);
    if (!facingRight_) {
        DrawRectangle((int)bx, (int)(by + 4), 8, 8, BLACK);
    } else {
        DrawRectangle((int)(bx + width_ - 8), (int)(by + 4), 8, 8, BLACK);
    }
}
