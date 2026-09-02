#pragma once
#include "BaseItem.h"

class Key : public BaseItem {
public:
    Key(Vector2 worldPos, float scale = 2.0f);
    void update(float dt) override;
    void render(float alpha) override;
    void onInteract(Entity& other) override;

    void setFollowing(Entity* target, int index) {
        followingTarget_ = target;
        keyIndex_ = index;
        itemState_ = ItemState::Active;
        setAnimation(ItemState::Active);
    }

private:
    Entity* followingTarget_ = nullptr;
    int keyIndex_ = 0;
};
