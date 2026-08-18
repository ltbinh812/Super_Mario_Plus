#include "Key.h"
#include "Entity.h"
#include "Player.h"
#include <iostream>

static const float BLOCK_SIZE = 32.0f;

Key::Key(Vector2 worldPos, float scale)
    : BaseItem(worldPos, BLOCK_SIZE, BLOCK_SIZE) 
{
    baseStats.gravityScale = 160.0f;
    runtimeStats.velocity = { ((rand() % 200) - 100) * 1.0f, -450.0f };
}

void Key::update(float dt) {
    BaseItem::update(dt);

    if (itemState_ == ItemState::Active && followingTarget_) {
        Player* p = dynamic_cast<Player*>(followingTarget_);
        if (p && p->getPartyInventory()) {
            if (p->getPartyInventory()->keys < keyIndex_) {
                itemState_ = ItemState::Used; // Self-destruct when used
                return;
            }
        }

        // Hovers 40 pixels above and 20 pixels behind the player
        float targetX = followingTarget_->getWorldStats().position.x;
        float offsetDir = followingTarget_->getWorldStats().isFacingRight ? -1.0f : 1.0f;
        targetX += offsetDir * (20.0f + keyIndex_ * 10.0f); // Space out multiple keys
        
        float targetY = followingTarget_->getWorldStats().position.y - 40.0f; 

        // Smooth lerp towards target position
        worldStats.position.x += (targetX - worldStats.position.x) * 5.0f * dt;
        worldStats.position.y += (targetY - worldStats.position.y) * 5.0f * dt;
    }
}

void Key::render(float alpha) {
    if (itemState_ == ItemState::Used) return;
    drawFrame("key_green.png");
}

void Key::onInteract(Entity& other) {
    if (itemState_ == ItemState::Active || itemState_ == ItemState::Used) return;
    if (pickupDelay_ > 0.0f) return;
    
    Player* p = dynamic_cast<Player*>(&other);
    if (p) {
        if (p->getPartyInventory()) {
            p->getPartyInventory()->keys += 1;
            keyIndex_ = p->getPartyInventory()->keys;
        }
        itemState_ = ItemState::Active;
        followingTarget_ = p;
        
        // Remove physics to allow hovering
        baseStats.gravityScale = 0.0f;
        runtimeStats.velocity = {0.0f, 0.0f};
        runtimeStats.collisionMask = 0;
        
        std::cout << "[Key] Picked up! Hovering behind player.\n";
    }
}
