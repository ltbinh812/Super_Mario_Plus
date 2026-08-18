#pragma once
#include "BaseItem.h"

class Flag : public BaseItem {
public:
    Flag(Vector2 worldPos, float scale = 2.0f);
    void update(float dt) override;
    void render(float alpha) override;
    void onInteract(Entity& other) override;
    bool isActive() const { return itemState_ == ItemState::Active; }
    float getRenderOffsetY() const override { return 0.0f; }
private:
    float animTimer_ = 0.0f;
    bool frameToggle_ = false;
};
