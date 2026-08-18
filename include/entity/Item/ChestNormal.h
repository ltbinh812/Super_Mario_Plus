#pragma once
#include "BaseItem.h"
#include <string>

// Drops: random Buff or Boom. Frame: chest_01/02/11 (random).
class ChestNormal : public BaseItem {
public:
    ChestNormal(Vector2 worldPos, float scale = 2.0f);
    void render(float alpha) override;
    void onInteract(Entity& other) override;
    float getRenderOffsetY() const override;

private:
    std::string frame_;
};
