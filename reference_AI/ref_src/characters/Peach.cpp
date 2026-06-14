#include "Peach.h"

Peach::Peach() {
    gravity_ = 900.0f; // Floats
}

void Peach::Draw() {
    Color pink = { 255, 192, 203, 255 };
    float bx = pos_.x;
    float by = pos_.y;
    DrawRectangle((int)bx, (int)by, (int)width_, (int)height_, pink);
    if (!facingRight_) {
        DrawRectangle((int)bx, (int)(by + 4), 8, 8, BLACK);
    } else {
        DrawRectangle((int)(bx + width_ - 8), (int)(by + 4), 8, 8, BLACK);
    }
}
