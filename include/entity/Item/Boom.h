#pragma once
#include "BaseItem.h"

// 3s countdown, alternates bomb.png/bomb_active.png every 0.5s, then explosion 5x3 blocks.
// Pivot of explosion: center of bottom edge.
class Boom : public BaseItem {
public:
    Boom(Vector2 worldPos, float scale = 2.0f);
    // Create a pre-activated thrown bomb with initial velocity
    Boom(Vector2 worldPos, Vector2 initVelocity);

    void activate();  // Start 3s countdown
    void update(float dt) override;
    void render(float alpha) override;
    void onInteract(Entity& other) override;
    void forceInteract(Entity& other) override;
    float getRenderOffsetY() const override;
    bool hasExploded() const { return exploded_; }

    // Returns the explosion world-rect (valid only after exploded_ == true)
    Rectangle getExplosionRect() const;

private:
    bool active_      = false;
    bool exploded_    = false;
    bool damageEmitted_ = false; // Ensures ExplosionDamage command is pushed exactly once
    float timer_      = 2.0f;
    float animTimer_  = 0.0f;
    bool frameToggle_ = false;
    float explosionTimer_ = 0.75f;
    AtlasAnimation explosionAnim_;
    static constexpr float TOGGLE_INTERVAL = 0.15f;
    static constexpr float EXPLODE_TIME    = 2.0f;
    static constexpr float EXPLOSION_SHOW  = 0.75f;
    // 5 blocks wide x 3 blocks tall (1 block = 32px)
    static constexpr float EXPL_W = 5 * 32.0f;
    static constexpr float EXPL_H = 3 * 32.0f;
};
