#include "Flag.h"
#include <iostream>

static const float BLOCK_SIZE = 32.0f;

Flag::Flag(Vector2 worldPos, float scale)
    : BaseItem(worldPos, BLOCK_SIZE * 3.0f, BLOCK_SIZE * 3.0f) {}

void Flag::update(float dt) {
    BaseItem::update(dt);
    if (itemState_ != ItemState::Active) return;
    animTimer_ += dt;
    if (animTimer_ >= 0.4f) {
        animTimer_ = 0.0f;
        frameToggle_ = !frameToggle_;
    }
}

void Flag::render(float alpha) {
    if (itemState_ == ItemState::Idle) {
        drawFrame("flag_off.png");
    } else {
        drawFrame(frameToggle_ ? "flag_green_a.png" : "flag_green_b.png");
    }
}

void Flag::onInteract(Entity& other) {
    if (itemState_ == ItemState::Active) return;
    itemState_ = ItemState::Active;
    std::cout << "[Flag] Activated!\n";
}
