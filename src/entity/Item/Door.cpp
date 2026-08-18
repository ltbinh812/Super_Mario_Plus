#include "Door.h"
#include "Entity.h"
#include "Player.h"
#include <iostream>

// Door: 5x4 blocks
static constexpr float DOOR_W = 5 * 32.0f;
static constexpr float DOOR_H = 4 * 32.0f;

Door::Door(Vector2 worldPos, float scale)
    : BaseItem(worldPos, DOOR_W, DOOR_H) {}

Rectangle Door::getSolidRect() const {
    if (isOpen()) return { 0, 0, 0, 0 };
    return { worldStats.position.x, worldStats.position.y - DOOR_H, DOOR_W, DOOR_H };
}

Rectangle Door::getHitbox() const {
    // Expand hitbox by 2 pixels on all sides so touching the solid edge triggers onInteract
    return { worldStats.position.x - 2.0f, worldStats.position.y - DOOR_H - 2.0f, DOOR_W + 4.0f, DOOR_H + 4.0f };
}

void Door::render(float alpha) {
    std::string frame = isOpen() ? "door_open.png" : "door_closed.png";
    Rectangle dest = {
        worldStats.position.x,
        worldStats.position.y - DOOR_H,
        DOOR_W, DOOR_H
    };
    drawFrameRect(frame, dest);
}

void Door::onInteract(Entity& other) {
    if (itemState_ == ItemState::Active) return;

    Player* p = dynamic_cast<Player*>(&other);
    if (p && p->getPartyInventory()) {
        auto inv = p->getPartyInventory();
        if (inv->keys > 0) {
            inv->keys--;
            itemState_ = ItemState::Active;
            std::cout << "[Door] Unlocked!\n";
        } else {
            std::cout << "[Door] Locked! Needs a key.\n";
        }
    }
}
