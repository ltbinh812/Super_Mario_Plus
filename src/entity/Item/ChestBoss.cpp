#include "ChestBoss.h"
#include "Entity.h"
#include <cstdlib>
#include <iostream>
#include <cmath>

static const float BLOCK_SIZE = 32.0f;

static const char* FRAMES[] = {
    "chest_03.png","chest_04.png","chest_07.png",
    "chest_08.png","chest_09.png","chest_10.png"
};

ChestBoss::ChestBoss(Vector2 worldPos, float scale)
    : BaseItem(worldPos, BLOCK_SIZE, BLOCK_SIZE)
{
    frame_ = FRAMES[rand() % 6];
}

void ChestBoss::render(float alpha) {
    Color tint = (itemState_ == ItemState::Active) ? Fade(WHITE, 0.5f) : WHITE;
    drawFrame(frame_, tint);
}

void ChestBoss::onInteract(Entity& other) {
    if (itemState_ == ItemState::Active) return;
    itemState_ = ItemState::Active;
    if (commandQueue) {
        SpawnCommand cmd;
        cmd.category = SpawnCategory::Item;
        cmd.itemIdentifier = "Key";
        cmd.position = { worldStats.position.x, worldStats.position.y - hitH_ };
        commandQueue->push(cmd);
    }
    std::cout << "[ChestBoss] Opened! Dropping Key.\n";
}

float ChestBoss::getRenderOffsetY() const {
    if (itemState_ == ItemState::Active) return 0.0f;
    return BaseItem::getRenderOffsetY();
}
