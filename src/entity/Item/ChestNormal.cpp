#include "ChestNormal.h"
#include "Entity.h"
#include <cstdlib>
#include <iostream>
#include <cmath>
#include "CommandQueue.h"

static const float HITBOX_W = 32.0f;
static const float HITBOX_H = 32.0f;

ChestNormal::ChestNormal(Vector2 worldPos, float scale)
    : BaseItem(worldPos, HITBOX_W, HITBOX_H)
{
    // 4 types of chests: 1 to 4
    chestType_ = (rand() % 4) + 1;
    
    std::string idleFrame = "normal_chest_" + std::to_string(chestType_) + "_idle";
    std::string activeFrame = "normal_chest_" + std::to_string(chestType_) + "_active";

    animations_[ItemState::Idle] = AtlasAnimation(idleFrame, 5, 0.1f, true);
    animations_[ItemState::Active] = AtlasAnimation(activeFrame, 5, 0.1f, false);
    
    setAnimation(ItemState::Idle);
}

void ChestNormal::render(float alpha) {
    if (!currentAnim_ || !currentAnim_->isValid()) return;

    const Texture2D& tex = currentAnim_->getTexture();
    if (tex.id == 0) return;

    Rectangle src = currentAnim_->getCurrentSourceRect();
    
    // Draw using the chest's frame size (96x64), centered on the 32x32 hitbox
    float drawW = 96.0f;
    float drawH = 64.0f;
    
    Rectangle dest = {
        worldStats.position.x - (drawW - hitW_) / 2.0f,
        worldStats.position.y - drawH + getRenderOffsetY(),
        drawW,
        drawH
    };
    
    DrawTexturePro(tex, src, dest, { 0, 0 }, 0.0f, WHITE);
}

void ChestNormal::onInteract(Entity& other) {
    if (itemState_ == ItemState::Active) return;
    itemState_ = ItemState::Active;
    setAnimation(ItemState::Active);

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
    return 0.0f;
}
