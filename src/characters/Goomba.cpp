#include "Goomba.h"
#include <cmath>

Goomba::Goomba() {
    width_ = 32.0f;
    height_ = 32.0f;
}

void Goomba::Init(float startX, float startY) {
    Character::Init(startX, startY);
    height_ = 32.0f;
    width_ = 32.0f;
    moveDirection_ = -1.0f;
    vel_.x = moveDirection_ * patrolSpeed_;
}

void Goomba::Update(float dt, float worldWidth) {
    // Simple AI: keep walking in one direction until hitting something
    vel_.x = moveDirection_ * patrolSpeed_;

    Character::Update(dt, worldWidth);
}

void Goomba::ResolveCollision(Rectangle platRect) {
    Rectangle pb = GetBounds();
    if (!CheckCollisionRecs(pb, platRect)) return;

    float overlapLeft   = (pb.x + pb.width)  - platRect.x;
    float overlapRight  = (platRect.x + platRect.width) - pb.x;
    float overlapTop    = (pb.y + pb.height)  - platRect.y;
    float overlapBottom = (platRect.y + platRect.height) - pb.y;

    bool fromLeft   = overlapLeft   < overlapRight && overlapLeft   < overlapTop && overlapLeft   < overlapBottom;
    bool fromRight  = overlapRight  < overlapLeft  && overlapRight  < overlapTop && overlapRight  < overlapBottom;
    bool fromTop    = overlapTop    < overlapBottom && overlapTop   < overlapLeft && overlapTop   < overlapRight;
    bool fromBottom = overlapBottom < overlapTop   && overlapBottom < overlapLeft && overlapBottom < overlapRight;

    if (fromTop) {
        pos_.y  = platRect.y - pb.height;
        vel_.y  = 0.0f;
        onGround_ = true;
    } else if (fromBottom) {
        pos_.y  = platRect.y + platRect.height;
        vel_.y  = fabsf(vel_.y) * 0.1f;
    } else if (fromLeft) {
        pos_.x  = platRect.x - pb.width;
        moveDirection_ = -1.0f; // Hit right wall, go left
    } else if (fromRight) {
        pos_.x  = platRect.x + platRect.width;
        moveDirection_ = 1.0f; // Hit left wall, go right
    }
}

void Goomba::Draw() {
    float bx = pos_.x;
    float by = pos_.y;
    
    // Simple drawing for a Goomba
    Color brown = { 139, 69, 19, 255 };
    Color lightBrown = { 205, 133, 63, 255 };
    Color black = { 0, 0, 0, 255 };
    Color white = { 255, 255, 255, 255 };

    // Mushroom head
    DrawCircle((int)(bx + width_/2), (int)(by + height_/2), (int)(width_/2), brown);
    
    // Stem/Body
    DrawRectangle((int)(bx + width_/4), (int)(by + height_/2), (int)(width_/2), (int)(height_/2), lightBrown);

    // Eyes
    float eyeOffY = height_ * 0.4f;
    DrawRectangle((int)(bx + width_ * 0.3f), (int)(by + eyeOffY), 4, 8, white);
    DrawRectangle((int)(bx + width_ * 0.6f), (int)(by + eyeOffY), 4, 8, white);
    DrawRectangle((int)(bx + width_ * 0.35f), (int)(by + eyeOffY + 2), 2, 4, black);
    DrawRectangle((int)(bx + width_ * 0.65f), (int)(by + eyeOffY + 2), 2, 4, black);

    // Feet (simple animation)
    float footSwing = sinf(GetTime() * 10.0f) * 4.0f;
    DrawCircle((int)(bx + width_ * 0.3f + footSwing), (int)(by + height_), 6, black);
    DrawCircle((int)(bx + width_ * 0.7f - footSwing), (int)(by + height_), 6, black);
}
