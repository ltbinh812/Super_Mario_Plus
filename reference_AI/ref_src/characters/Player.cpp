#include "Player.h"
#include <cmath>

Player::Player() {
    width_ = 32.0f;
    height_ = 52.0f;
    crouching_ = false;
    ChangeState(std::make_unique<PlayerIdleState>());
}

Player::~Player() {}

void Player::Init(float startX, float startY) {
    Character::Init(startX, startY);
    height_ = 52.0f;
    crouching_ = false;
    ChangeState(std::make_unique<PlayerIdleState>());
}

void Player::Update(float dt, float worldWidth) {
    auto config = GetInputConfig();
    bool crouchInput = IsKeyDown(config.crouchKey);
    if (crouchInput && onGround_) {
        if (!crouching_) {
            crouching_ = true;
            pos_.y += (height_ - 32.0f); // Shift down
            height_ = 32.0f;
        }
        vel_.x *= 0.85f;
    } else {
        if (crouching_) {
            crouching_ = false;
            pos_.y -= (52.0f - height_); // Shift up
            height_ = 52.0f;
        }
    }

    if (crouching_) {
        // Skip normal state update if crouching to prevent moving
        ApplyPhysics(dt, worldWidth);
    } else {
        Character::Update(dt, worldWidth);
    }
}

void Player::Draw() {
    Vector2 center = {
        pos_.x + width_  * 0.5f,
        pos_.y + height_ * 0.5f
    };
    DrawMarioCharacter(center, 1.0f);
}

void Player::DrawMarioCharacter(Vector2 center, float scale) {
    float dir    = facingRight_ ? 1.0f : -1.0f;
    bool  crouch = crouching_;
    float squish = crouch ? 0.6f : 1.0f;

    float bodyH   = 22.0f * scale * squish;
    float bodyW   = 24.0f * scale;
    float headR   = 12.0f * scale;
    float hatW    = 26.0f * scale;
    float hatH    = 10.0f * scale * squish;
    float legH    = crouch ? 6.0f * scale : 14.0f * scale;
    float legW    = 10.0f * scale;
    float shoeH   = 6.0f  * scale;
    float armW    = 10.0f * scale;
    float armH    = 16.0f * scale * squish;

    float legSwing = 0.0f;
    // Check if running/walking to swing legs
    if (!crouch && fabsf(vel_.x) > 10.0f && onGround_) {
        legSwing = sinf(GetTime() * 12.0f) * 6.0f * scale;
    }

    Color red    = { 200,  40,  40, 255 };
    Color blue   = {  40,  80, 180, 255 };
    Color skin   = { 255, 200, 140, 255 };
    Color brown  = { 140,  80,  20, 255 };
    Color darkBr = { 100,  55,  10, 255 };
    Color white  = { 255, 255, 255, 255 };

    float bx = center.x - bodyW * 0.5f;
    float by = center.y - bodyH * 0.5f;

    float lLegX = bx + bodyW * 0.1f;
    float rLegX = bx + bodyW * 0.5f;
    float legY  = by + bodyH;
    DrawRectangle((int)(lLegX),                (int)(legY + legSwing),       (int)legW, (int)legH, blue);
    DrawRectangle((int)(rLegX),                (int)(legY - legSwing),       (int)legW, (int)legH, blue);
    DrawRectangle((int)(lLegX - 2 * scale),   (int)(legY + legH + legSwing),(int)(legW + 6 * scale), (int)shoeH, darkBr);
    DrawRectangle((int)(rLegX - 2 * scale),   (int)(legY + legH - legSwing),(int)(legW + 6 * scale), (int)shoeH, darkBr);

    DrawRectangle((int)bx, (int)by, (int)bodyW, (int)bodyH, red);
    DrawRectangle((int)(bx + bodyW * 0.25f), (int)(by + bodyH * 0.3f),
                  (int)(bodyW * 0.5f), (int)(bodyH * 0.45f), blue);

    float armDir = dir;
    float armSwing = (!crouch && fabsf(vel_.x) > 10.0f && onGround_) ? sinf(GetTime() * 12.0f) * 5.0f * scale : 0.0f;
    DrawRectangle((int)(bx + bodyW - 2 * scale),
                  (int)(by + 2 * scale + armSwing * armDir),
                  (int)armW, (int)armH, red);
    DrawRectangle((int)(bx - armW + 2 * scale),
                  (int)(by + 2 * scale - armSwing * armDir),
                  (int)armW, (int)armH, red);
    Color glove = { 240, 230, 210, 255 };
    DrawCircle((int)(bx + bodyW + armW * 0.1f), (int)(by + armH + armSwing * armDir + 2),
               (int)(armW * 0.55f), glove);
    DrawCircle((int)(bx - armW * 0.6f), (int)(by + armH - armSwing * armDir + 2),
               (int)(armW * 0.55f), glove);

    float hx = center.x;
    float hy = by - headR;
    DrawCircle((int)hx, (int)hy, (int)headR, skin);

    float eyeOffX = dir * headR * 0.35f;
    DrawCircle((int)(hx + eyeOffX), (int)(hy - headR * 0.1f), (int)(2.5f * scale), white);
    DrawCircle((int)(hx + eyeOffX + dir * 1 * scale), (int)(hy - headR * 0.1f), (int)(1.5f * scale), { 20, 20, 20, 255 });
    DrawCircle((int)(hx + dir * headR * 0.2f), (int)(hy + headR * 0.25f), (int)(3.5f * scale), { 230, 150, 100, 255 });
    DrawRectangle((int)(hx - headR * 0.7f), (int)(hy + headR * 0.35f),
                  (int)(headR * 1.4f), (int)(3 * scale), brown);

    DrawRectangle((int)(hx - hatW * 0.5f), (int)(hy - headR * 0.15f),
                  (int)hatW, (int)(5 * scale), red);
    DrawRectangle((int)(hx - hatW * 0.4f), (int)(hy - headR * 0.15f - hatH),
                  (int)(hatW * 0.8f), (int)hatH, red);
    DrawRectangle((int)(hx - 5 * scale), (int)(hy - headR * 0.15f - hatH * 0.7f),
                  (int)(10 * scale), (int)(hatH * 0.5f), white);
}
