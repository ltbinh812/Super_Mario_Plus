#pragma once
#include "BaseItem.h"
#include <string>

// Drops: Key. Frame: chest_03/04/07/08/09/10 (random).
class ChestBoss : public BaseItem {
public:
    ChestBoss(Vector2 worldPos, float scale = 2.0f);
    void render(float alpha) override;
    void onInteract(Entity& other) override;
    float getRenderOffsetY() const override;

private:
    std::string frame_;
};
