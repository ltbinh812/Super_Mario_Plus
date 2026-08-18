#include "Coin.h"
#include "Entity.h"
#include "Player.h"
#include <iostream>
#include <cmath>
#include <cstdlib>

static const float BLOCK_SIZE = 32.0f;

Coin::Coin(Vector2 worldPos, float scale)
    : BaseItem(worldPos, BLOCK_SIZE, BLOCK_SIZE)
{
    baseStats.gravityScale = 160.0f;
    // Pop up if spawned dynamically
    runtimeStats.velocity = { ((rand() % 200) - 100) * 1.0f, -450.0f };
}

void Coin::update(float dt) {
    BaseItem::update(dt);
}

void Coin::render(float alpha) {
    if (itemState_ == ItemState::Used) return;
    drawFrame("coin_gold.png");
}

void Coin::onInteract(Entity& other) {
    if (itemState_ == ItemState::Used) return;
    if (pickupDelay_ > 0.0f) return;
    
    Player* p = dynamic_cast<Player*>(&other);
    if (p) {
        if (p->getPartyInventory()) p->getPartyInventory()->coins += 1;
        itemState_ = ItemState::Used;
        std::cout << "[Coin] Collected! Coins +1\n";
    }
}
