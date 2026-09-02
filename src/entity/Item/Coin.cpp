#include "Coin.h"
#include "Entity.h"
#include "Player.h"
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <raymath.h>
#include "infrastructure/AssetManager.h"
static const float BLOCK_SIZE = 32.0f;

Coin::Coin(Vector2 worldPos, float scale)
    : BaseItem(worldPos, BLOCK_SIZE, BLOCK_SIZE)
{
    // gravityScale = 160 để Coin đặt trong map rơi xuống đất tự nhiên.
    // Velocity KHÔNG tự set ở đây — khi spawn từ quái chết hoặc chậu kho, createDynamic()
    // sẽ gọi launchAsDrop() để set velocity bật lên đúng 1 block.
    baseStats.gravityScale = 160.0f;

    // Set up default animation
    animations_[ItemState::Idle] = AtlasAnimation("coin_anim", 6, 0.1f);
    setAnimation(ItemState::Idle);
}

void Coin::update(float dt) {
    BaseItem::update(dt);
}

void Coin::process(const std::vector<Player*>& players) {
    Player* targetPlayer = nullptr;
    float minDst = 160.0f; // Pull radius (5 blocks)
    
    for (Player* p : players) {
        if (p && p->getBuffManager().hasGoldMagnet()) {
            float dst = Vector2Distance(p->getWorldStats().position, worldStats.position);
            if (dst < minDst) {
                minDst = dst;
                targetPlayer = p;
            }
        }
    }

    if (targetPlayer) {
        Vector2 dir = Vector2Normalize(Vector2Subtract(targetPlayer->getWorldStats().position, worldStats.position));
        float pullSpeed = 400.0f;
        runtimeStats.velocity.x = dir.x * pullSpeed;
        runtimeStats.velocity.y = dir.y * pullSpeed;
        baseStats.gravityScale = 0.0f;
    } else {
        baseStats.gravityScale = 160.0f;
    }
}

void Coin::render(float alpha) {
    if (itemState_ == ItemState::Used) return;
    
    // Draw using animation if it has frames, otherwise fallback to drawFrame for the static image
    if (currentAnim_ && currentAnim_->isValid()) {
        drawAnim();
    } else {
        drawFrame("coin_gold.png");
    }
}

void Coin::onInteract(Entity& other) {
    if (itemState_ == ItemState::Used) return;
    if (pickupDelay_ > 0.0f) return;
    
    Player* p = dynamic_cast<Player*>(&other);
    if (p) {
        if (p->getPartyInventory()) p->getPartyInventory()->coins += 1;
        itemState_ = ItemState::Used;
        std::cout << "[Coin] Collected! Coins +1\n";
        PlaySound(AssetManager::getInstance().getSound("coin_sound"));
    }
}
