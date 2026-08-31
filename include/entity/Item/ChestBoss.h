#pragma once
#include "BaseItem.h"

// Drops: Key. Uses 3 random boss chest types (2-frame idle, 3-frame active).
class ChestBoss : public BaseItem {
public:
    ChestBoss(Vector2 worldPos, float scale = 2.0f);
    void render(float alpha) override;
    void onInteract(Entity& other) override;
    float getRenderOffsetY() const override;

private:
    int chestType_;
};
