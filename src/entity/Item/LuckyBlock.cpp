#include "LuckyBlock.h"
#include "Entity.h"
#include <iostream>

static const float BLOCK_SIZE = 32.0f;

LuckyBlock::LuckyBlock(Vector2 worldPos, float scale)
    : BaseItem(worldPos, BLOCK_SIZE, BLOCK_SIZE) 
{
    originalY_ = worldPos.y;
}

void LuckyBlock::update(float dt) {
    BaseItem::update(dt);
    if (bounceTimer_ >= 0.0f) {
        bounceTimer_ -= dt;
    }
}

float LuckyBlock::getRenderOffsetY() const {
    if (bounceTimer_ > 0.1f) {
        return -(0.2f - bounceTimer_) * 10.0f * 16.0f;
    } else if (bounceTimer_ > 0.0f) {
        return -bounceTimer_ * 10.0f * 16.0f;
    }
    return 0.0f;
}

Rectangle LuckyBlock::getSolidRect() const {
    return BaseItem::getHitbox(); 
}

Rectangle LuckyBlock::getHitbox() const {
    Rectangle rect = BaseItem::getHitbox();
    rect.x -= 2.0f;
    rect.y -= 2.0f;
    rect.width += 4.0f;
    rect.height += 4.0f;
    return rect;
}

void LuckyBlock::render(float alpha) {
    // Offset rendering for bounce animation
    Vector2 oldPos = worldStats.position;
    worldStats.position.y += getRenderOffsetY();
    
    // Swap assets as requested by user
    drawFrame(isTriggered() ? "block_exclamation.png" : "block_exclamation_active.png");
    
    worldStats.position = oldPos;
}

void LuckyBlock::onInteract(Entity& other) {
    if (isTriggered()) return;
    
    // In Raylib, if origin is bottom-center, originalY_ is the block's bottom Y.
    // The player's top Y is other.getHitbox().y
    float playerTop = other.getHitbox().y;
    float blockBottom = originalY_;
    
    // When hitting ceiling, player's velocity.y is reset to 0, and their top is aligned to the block's bottom.
    auto& vel = other.getRuntimeStatsMutable().velocity;
    if (std::abs(playerTop - blockBottom) < 2.1f && vel.y >= 0.0f) {
        itemState_ = ItemState::Active;
        bounceTimer_ = 0.2f;
        
        if (commandQueue) {
            SpawnCommand cmd;
            cmd.category = SpawnCategory::Item;
            int randVal = rand() % 3;
            if (randVal == 0) cmd.itemIdentifier = "Coin";
            else if (randVal == 1) cmd.itemIdentifier = "Boom";
            else cmd.itemIdentifier = "Buff";
            cmd.position = {worldStats.position.x, originalY_ - 32.0f};
            commandQueue->push(cmd);
        }
        
        std::cout << "[LuckyBlock] Hit from below! Bouncing & Spawning item...\n";
    }
}
