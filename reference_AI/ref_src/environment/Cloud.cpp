#include "Cloud.h"

Cloud::Cloud(Vector2 pos, float speed, float scale) : pos_(pos), speed_(speed), scale_(scale) {}

void Cloud::Update(float dt, float worldWidth) {
    pos_.x -= speed_ * dt;
    if (pos_.x < -200) pos_.x = worldWidth + 100;
}

void Cloud::Draw() const {
    Color col  = { 255, 255, 255, 230 };
    Color col2 = { 220, 240, 255, 200 };
    int x = (int)pos_.x, y = (int)pos_.y;
    DrawCircle(x,                     y,                     (int)(28 * scale_), col2);
    DrawCircle(x + (int)(28 * scale_), y + (int)(8  * scale_), (int)(22 * scale_), col);
    DrawCircle(x - (int)(28 * scale_), y + (int)(8  * scale_), (int)(20 * scale_), col);
    DrawCircle(x + (int)(10 * scale_), y - (int)(16 * scale_), (int)(24 * scale_), col);
    DrawCircle(x - (int)(10 * scale_), y - (int)(12 * scale_), (int)(20 * scale_), col);
}
