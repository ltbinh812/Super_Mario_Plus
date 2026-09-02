#include "ChestBoss.h"
#include "Entity.h"
#include <cstdlib>
#include <iostream>
#include "CommandQueue.h"
#include "infrastructure/AssetManager.h"

static const float HITBOX_W = 32.0f;
static const float HITBOX_H = 32.0f;

ChestBoss::ChestBoss(Vector2 worldPos, float scale)
    : BaseItem(worldPos, HITBOX_W, HITBOX_H)
{
    // 3 types of boss chests: 1 to 3
    chestType_ = (rand() % 3) + 1;
    
    std::string idleFrame = "boss_chest_" + std::to_string(chestType_) + "_idle";
    std::string activeFrame = "boss_chest_" + std::to_string(chestType_) + "_active";

    // Idle has 2 frames, Active has 3 frames
    animations_[ItemState::Idle] = AtlasAnimation(idleFrame, 2, 0.15f, true);
    animations_[ItemState::Active] = AtlasAnimation(activeFrame, 3, 0.1f, false);
    
    setAnimation(ItemState::Idle);
}

void ChestBoss::render(float alpha) {
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

void ChestBoss::onInteract(Entity& other) {
    if (itemState_ == ItemState::Active) return;
    itemState_ = ItemState::Active;
    setAnimation(ItemState::Active);

    if (commandQueue) {
        SpawnCommand cmd;
        cmd.category = SpawnCategory::Item;
        cmd.itemIdentifier = "Key";
        cmd.position = { worldStats.position.x, worldStats.position.y - hitH_ };
        commandQueue->push(cmd);
    }
    std::cout << "[ChestBoss] Opened! Dropping Key.\n";
    PlaySound(AssetManager::getInstance().getSound("chest_open_sound"));
}

float ChestBoss::getRenderOffsetY() const {
    return 0.0f; // Floating effect disabled as requested
}
