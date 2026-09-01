#pragma once
#include "Entity.h"
#include "FireballConfig.h"
#include "Animation.h"
#include <memory>

class Fireball : public Entity {
private:
    bool isActive = true;
    float lifetime;
    int attackPower;
    float curveAmplitude;
    float curveFrequency;
    float elapsedTime = 0.0f;
    float originY;
    std::unique_ptr<Animation> animation;
    Entity* spawner;
    float hitboxOffsetX = 0.0f;
    float hitboxOffsetY = 0.0f;
    
public:
    // We pass initial position, facing direction, and per-character config
    Fireball(Vector2 startPos, bool isFacingRight, const FireballConfig& config, Entity* spawner);

    void update(float dt) override;
    void render(float alpha) override;

    void onHitWall(bool isRightWall, bool isCliff = false) override;
    void onCollide(Entity& other) override;

    // Combat interface
    bool hasActiveHitbox() const override;
    Hitbox getActiveHitbox() override;
    Rectangle getOffsetHitbox() const;
    void takeDamage(int damage, float knockbackDirX = 0.0f, bool forceInterrupt = true) override;

    bool getIsActive() const override { return isActive; }
    void deactivate() override { isActive = false; }
};
