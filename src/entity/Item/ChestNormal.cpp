#include "ChestNormal.h"
#include "Entity.h"
#include <cstdlib>
#include <iostream>
#include <cmath>
#include "CommandQueue.h"

static const float BLOCK_SIZE = 32.0f;

static const char* FRAMES[] = { "chest_01.png", "chest_02.png", "chest_11.png" };

ChestNormal::ChestNormal(Vector2 worldPos, float scale)
    : BaseItem(worldPos, BLOCK_SIZE, BLOCK_SIZE)
{
    frame_ = FRAMES[rand() % 3];
}

void ChestNormal::render(float alpha) {
    Color tint = (itemState_ == ItemState::Active) ? Fade(WHITE, 0.5f) : WHITE;
    drawFrame(frame_, tint);
}

void ChestNormal::onInteract(Entity& other) {
    if (itemState_ == ItemState::Active) return;
    itemState_ = ItemState::Active;

    if (commandQueue) {
        SpawnCommand cmd;
        cmd.category = SpawnCategory::Item;
        
        int randVal = rand() % 3;
        if (randVal == 0) cmd.itemIdentifier = "Coin";
        else if (randVal == 1) cmd.itemIdentifier = "Boom";
        else cmd.itemIdentifier = "Buff";

        // Spawn it slightly above the chest
        cmd.position = { worldStats.position.x, worldStats.position.y - hitH_ };
        commandQueue->push(cmd);
    }

    std::cout << "[ChestNormal] Opened! Spawning random item.\n";
}

float ChestNormal::getRenderOffsetY() const {
    if (itemState_ == ItemState::Active) return 0.0f;
    return BaseItem::getRenderOffsetY();
}
