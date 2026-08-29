#pragma once
#include "BaseItem.h"

// Drops: random Buff or Boom. Uses 4 random chest types with 5-frame animations.
class ChestNormal : public BaseItem {
public:
    ChestNormal(Vector2 worldPos, float scale = 2.0f);
    void render(float alpha) override;
    void onInteract(Entity& other) override;
    float getRenderOffsetY() const override;

private:
    int chestType_;
};
