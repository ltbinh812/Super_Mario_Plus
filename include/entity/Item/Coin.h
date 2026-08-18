#pragma once
#include "BaseItem.h"

class Coin : public BaseItem {
public:
    Coin(Vector2 worldPos, float scale = 2.0f);
    void update(float dt) override;
    void render(float alpha) override;
    void onInteract(Entity& other) override;
};
