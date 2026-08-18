#pragma once
#include "BaseItem.h"
#include <string>

// Door: 3x5 blocks. Closed=Solid, Open=passthrough.
class Door : public BaseItem {
public:
    Door(Vector2 worldPos, float scale = 2.0f);
    void render(float alpha) override;
    void onInteract(Entity& other) override;
    bool isOpen() const { return itemState_ == ItemState::Active; }
    float getRenderOffsetY() const override { return 0.0f; }
    
    Rectangle getSolidRect() const override;
    Rectangle getHitbox() const override;
};
