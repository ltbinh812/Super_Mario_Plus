#include "Wario.h"

Wario::Wario() {
    gravity_ = 1500.0f; // Heavy
    friction_ = 6.0f; // Slide less
}

void Wario::Draw() {
    Color yellow = { 255, 255, 0, 255 };
    Color purple = { 128, 0, 128, 255 };
    float bx = pos_.x;
    float by = pos_.y;
    DrawRectangle((int)bx, (int)by, (int)width_, (int)height_, yellow);
    DrawRectangle((int)bx, (int)(by + height_/2), (int)width_, (int)(height_/2), purple);
    if (!facingRight_) {
        DrawRectangle((int)bx, (int)(by + 4), 8, 8, BLACK);
    } else {
        DrawRectangle((int)(bx + width_ - 8), (int)(by + 4), 8, 8, BLACK);
    }
}
