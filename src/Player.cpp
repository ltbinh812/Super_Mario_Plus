#include "Player.h"
#include <cmath>

Player::Player() : width_(32.0f), height_(52.0f), onGround_(false), facingRight_(true), state_(PlayerState::IDLE) {
    pos_ = {0, 0};
    vel_ = {0, 0};
}

Player::~Player() {}

void Player::Init(float startX, float startY) {
    pos_ = { startX, startY };
    vel_ = { 0.0f, 0.0f };
    onGround_ = false;
    facingRight_ = true;
    state_ = PlayerState::IDLE;
}

void Player::Update(float dt, float worldWidth) {
    HandleInput(dt);
    ApplyPhysics(dt, worldWidth);
}

void Player::HandleInput(float dt) {
    bool crouching = (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN));

    if (crouching && onGround_) {
        state_  = PlayerState::CROUCHING;
        height_ = 32.0f;
        vel_.x *= 0.85f;
        return;
    } else {
        height_ = 52.0f;
    }

    bool running = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
    float targetSpeed = running ? RUN_SPEED : WALK_SPEED;

    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
        vel_.x -= targetSpeed * dt * FRICTION;
        if (vel_.x < -targetSpeed) vel_.x = -targetSpeed;
        facingRight_ = false;
    } else if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
        vel_.x += targetSpeed * dt * FRICTION;
        if (vel_.x > targetSpeed) vel_.x = targetSpeed;
        facingRight_ = true;
    } else {
        vel_.x *= (1.0f - dt * FRICTION * 0.5f);
        if (fabsf(vel_.x) < 2.0f) vel_.x = 0.0f;
    }

    bool jumpPressed = IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP);
    if (jumpPressed && onGround_) {
        vel_.y = JUMP_FORCE;
        onGround_ = false;
    }
    bool jumpHeld = IsKeyDown(KEY_SPACE) || IsKeyDown(KEY_W) || IsKeyDown(KEY_UP);
    if (jumpHeld && vel_.y < -200.0f) {
        vel_.y += GRAVITY * dt * 0.4f;
    }

    if (!onGround_) {
        state_ = (vel_.y < 0.0f) ? PlayerState::JUMPING : PlayerState::FALLING;
    } else if (fabsf(vel_.x) > 10.0f) {
        state_ = PlayerState::WALKING;
    } else {
        state_ = PlayerState::IDLE;
    }
}

void Player::ApplyPhysics(float dt, float worldWidth) {
    if (!onGround_) {
        vel_.y += GRAVITY * dt;
        if (vel_.y > MAX_FALL) vel_.y = MAX_FALL;
    }
    pos_.x += vel_.x * dt;
    pos_.y += vel_.y * dt;

    if (pos_.x < 0) pos_.x = 0;
    if (pos_.x + width_ > worldWidth) pos_.x = worldWidth - width_;

    if (pos_.y > 720 + 200) { // Respawn threshold
        pos_ = { 200.0f, 720 - 80.0f - height_ };
        vel_ = { 0, 0 };
    }
}

void Player::ResolveCollision(Rectangle platRect) {
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
        vel_.x  = 0.0f;
    } else if (fromRight) {
        pos_.x  = platRect.x + platRect.width;
        vel_.x  = 0.0f;
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
    bool  crouch = (state_ == PlayerState::CROUCHING);
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
    if (state_ == PlayerState::WALKING) {
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
    float armSwing = (state_ == PlayerState::WALKING) ? sinf(GetTime() * 12.0f) * 5.0f * scale : 0.0f;
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
