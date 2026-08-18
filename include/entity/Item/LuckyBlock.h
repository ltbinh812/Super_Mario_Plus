#pragma once
#include "BaseItem.h"

// Both states are solid. Triggered from below by player headbutt.
class LuckyBlock : public BaseItem {
public:
    LuckyBlock(Vector2 worldPos, float scale = 2.0f);
    void render(float alpha) override;
    void onInteract(Entity& other) override;
    bool isTriggered() const { return itemState_ == ItemState::Active; }
    void update(float dt) override;
    float getRenderOffsetY() const override;
    Rectangle getSolidRect() const override;
    Rectangle getHitbox() const override;
private:
    float bounceTimer_ = -1.0f;
    float originalY_ = 0.0f;
};
