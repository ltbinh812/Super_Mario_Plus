#ifndef CHARACTER_H
#define CHARACTER_H

#include "Entity.h"
#include "CharacterState.h"
#include "AbilityStrategy.h"
#include <memory>

struct InputConfig {
    int leftKey = 0;
    int rightKey = 0;
    int jumpKey = 0;
    int crouchKey = 0;
    int skill1Key = 0;
    int skill2Key = 0;
};

class Character : public Entity {
public:
    Character();
    virtual ~Character();

    virtual void Init(float startX, float startY);
    virtual void Update(float dt, float worldWidth) override;
    virtual void Draw() override;
    virtual void ResolveCollision(Rectangle otherBounds) override;

    Rectangle GetBounds() const override { return { pos_.x, pos_.y, width_, height_ }; }

    void ChangeState(std::unique_ptr<CharacterState> newState);
    
    void SetAbility1(std::unique_ptr<AbilityStrategy> newAbility);
    void SetAbility2(std::unique_ptr<AbilityStrategy> newAbility);
    void UseAbility1();
    void UseAbility2();

    void SetInputConfig(const InputConfig& config) { inputConfig_ = config; }
    const InputConfig& GetInputConfig() const { return inputConfig_; }

    // Setters/Getters useful for States/Physics
    void SetVelocityY(float vy) { vel_.y = vy; }
    void SetVelocityX(float vx) { vel_.x = vx; }
    float GetVelocityY() const { return vel_.y; }
    float GetVelocityX() const { return vel_.x; }

    void SetOnGround(bool og) { onGround_ = og; }
    bool IsOnGround() const { return onGround_; }

    void SetFacingRight(bool fr) { facingRight_ = fr; }
    bool IsFacingRight() const { return facingRight_; }

    void SetHeight(float h) { height_ = h; }
    float GetHeight() const { return height_; }
    void SetWidth(float w) { width_ = w; }
    float GetWidth() const { return width_; }

    // Constants for physics (can be overridden or modified per character later)
    float GetGravity() const { return gravity_; }
    float GetJumpForce() const { return jumpForce_; }
    float GetWalkSpeed() const { return walkSpeed_; }
    float GetRunSpeed() const { return runSpeed_; }
    float GetFriction() const { return friction_; }
    float GetMaxFall() const { return maxFall_; }

protected:
    virtual void ApplyPhysics(float dt, float worldWidth);

    std::unique_ptr<CharacterState> currentState_;
    std::unique_ptr<AbilityStrategy> ability1_;
    std::unique_ptr<AbilityStrategy> ability2_;

    InputConfig inputConfig_;

    bool onGround_;
    bool facingRight_;

    float gravity_ = 1800.0f;
    float jumpForce_ = -680.0f;
    float walkSpeed_ = 260.0f;
    float runSpeed_ = 420.0f;
    float friction_ = 18.0f;
    float maxFall_ = 900.0f;
};

#endif
