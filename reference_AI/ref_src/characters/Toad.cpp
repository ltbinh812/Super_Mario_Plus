#include "Toad.h"

Toad::Toad() {
    walkSpeed_ = 350.0f; // Fast
    runSpeed_ = 550.0f;
}

void Toad::Draw() {
    Color white = { 255, 255, 255, 255 };
    Color blue = { 0, 0, 255, 255 };
    float bx = pos_.x;
    float by = pos_.y;
    DrawRectangle((int)bx, (int)by, (int)width_, (int)height_, white);
    DrawRectangle((int)bx, (int)(by + height_/2), (int)width_, (int)(height_/2), blue);
    if (!facingRight_) {
        DrawRectangle((int)bx, (int)(by + 4), 8, 8, BLACK);
    } else {
        DrawRectangle((int)(bx + width_ - 8), (int)(by + 4), 8, 8, BLACK);
    }
}
