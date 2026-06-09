#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include "raymath.h"

enum class PlayerState { IDLE, WALKING, JUMPING, FALLING, CROUCHING };

class Player {
public:
    Player();
    ~Player();

    void Init(float startX, float startY);
    void Update(float dt, float worldWidth);
    void Draw();

    void ResolveCollision(Rectangle platRect);
    void ResetGroundState() { onGround_ = false; }

    Vector2 GetPosition() const { return pos_; }
    float GetWidth() const { return width_; }
    float GetHeight() const { return height_; }
    Rectangle GetBounds() const { return { pos_.x, pos_.y, width_, height_ }; }
    
    void SetPosition(Vector2 p) { pos_ = p; }
    void SetVelocityY(float vy) { vel_.y = vy; }
    void SetVelocityX(float vx) { vel_.x = vx; }
    void SetOnGround(bool og) { onGround_ = og; }
    float GetVelocityY() const { return vel_.y; }

private:
    void HandleInput(float dt);
    void ApplyPhysics(float dt, float worldWidth);
    void DrawMarioCharacter(Vector2 center, float scale);

    Vector2 pos_;
    Vector2 vel_;
    float width_;
    float height_;
    bool onGround_;
    bool facingRight_;
    PlayerState state_;

    static constexpr float GRAVITY      = 1800.0f;
    static constexpr float JUMP_FORCE   = -680.0f;
    static constexpr float WALK_SPEED   = 260.0f;
    static constexpr float RUN_SPEED    = 420.0f;
    static constexpr float FRICTION     = 18.0f;
    static constexpr float MAX_FALL     = 900.0f;
};

#endif
