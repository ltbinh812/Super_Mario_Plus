#pragma once
#include "Entity.h"
#include "Animation.h"
#include "ExplosionConfig.h"
#include <memory>

class Explosion : public Entity {
private:
    float lifetime;
    int attackPower;
    float hitboxW;
    float hitboxH;
    bool isActive = true;
    Entity* spawner;
    std::unique_ptr<Animation> animation;

public:
    Explosion(Vector2 startPos, bool isFacingRight, const ExplosionConfig& config, Entity* spawner);

    void update(float dt) override;
    void render(float alpha) override;

    bool hasActiveHitbox() const override;
    Hitbox getActiveHitbox() override;

    void onHitWall(bool isRightWall) override {}
    void onCollide(Entity& other) override {}
    void takeDamage(int damage, float knockbackDirX = 0.0f, bool forceInterrupt = true) override {}

    bool getIsActive() const override { return isActive; }
    void deactivate() override { isActive = false; }
};
