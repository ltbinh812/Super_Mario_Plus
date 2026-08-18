#pragma once
#include "BaseItem.h"

class Key : public BaseItem {
public:
    Key(Vector2 worldPos, float scale = 2.0f);
    void update(float dt) override;
    void render(float alpha) override;
    void onInteract(Entity& other) override;

private:
    Entity* followingTarget_ = nullptr;
    int keyIndex_ = 0;
};
