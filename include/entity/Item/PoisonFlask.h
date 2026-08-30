#pragma once
#include "BaseItem.h"

class PoisonFlask : public BaseItem {
public:
    PoisonFlask(Vector2 worldPos, float scale = 2.0f);
    PoisonFlask(Vector2 worldPos, Vector2 initVelocity);

    void activate();
    void update(float dt) override;
    void render(float alpha) override;
    void onInteract(Entity& other) override;
    void forceInteract(Entity& other) override;
    float getRenderOffsetY() const override;
    bool hasExploded() const { return exploded_; }

private:
    bool active_      = false;
    bool exploded_    = false;
    bool damageEmitted_ = false;
    float timer_      = 2.0f;
    float animTimer_  = 0.0f;
    bool frameToggle_ = false;
    static constexpr float TOGGLE_INTERVAL = 0.15f;
    static constexpr float EXPLODE_TIME    = 2.0f;
};
