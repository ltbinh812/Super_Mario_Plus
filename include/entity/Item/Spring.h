#pragma once
#include "BaseItem.h"

enum class SpringDir { Down, Up, Left, Right };

// Spring: Static solid obstacle. On player contact from the trigger side, applies a launch force.
// Both idle and "out" (extended) states are solid.
class Spring : public BaseItem {
public:
    Spring(Vector2 worldPos, SpringDir dir, float scale = 2.0f);
    void update(float dt) override;
    void render(float alpha) override;
    void onInteract(Entity& other) override;
    Rectangle getSolidRect() const override;
    Rectangle getHitbox() const override;
    float getRenderOffsetY() const override { return 0.0f; }

private:
    SpringDir dir_;
    float animTimer_ = 0.0f;
    bool triggered_ = false;
    static constexpr float LAUNCH_FORCE = 1010.0f;
    static constexpr float ANIM_DURATION = 0.25f;

    std::string idleFrame() const;
    std::string outFrame() const;
};
