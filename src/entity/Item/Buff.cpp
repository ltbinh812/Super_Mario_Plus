#include "Buff.h"
#include "Entity.h"
#include "SpeedBuff.h"
#include "StrengthBuff.h"
#include "ShieldBuff.h"
#include "JumpBuff.h"
#include "InvisiBuff.h"
#include "GoldMagnetBuff.h"
#include "TimeStopBuff.h"
#include "RandomBuff.h"
#include "Player.h"
#include <iostream>
#include <cstdlib>
#include <cmath>

static const float BLOCK_SIZE = 32.0f;

Buff::Buff(Vector2 worldPos, float scale, const std::string& specificType)
    : BaseItem(worldPos, BLOCK_SIZE, BLOCK_SIZE)
{
    baseStats.gravityScale = 160.0f;
    runtimeStats.velocity = { ((rand() % 200) - 100) * 1.0f, -450.0f };
    if (specificType == "Meat") effect_ = std::make_unique<StrengthBuff>();
    else if (specificType == "Armor") effect_ = std::make_unique<ShieldBuff>();
    else if (specificType == "Spell") effect_ = std::make_unique<TimeStopBuff>();
    else if (specificType == "Vorpal_blade") effect_ = std::make_unique<SpeedBuff>();
    else {
        // Just make it a RandomBuff
        effect_ = std::make_unique<RandomBuff>();
    }
}

void Buff::render(float alpha) {
    if (itemState_ == ItemState::Used) return;
    drawFrame(effect_->getFrameName());
}

void Buff::onInteract(Entity& other) {
    if (itemState_ == ItemState::Active || itemState_ == ItemState::Used) return;
    if (pickupDelay_ > 0.0f) return;

    Player* p = dynamic_cast<Player*>(&other);
    if (p) {
        if (p->getRuntimeStats().storedItemSlot.empty()) {
            std::string buffName = effect_->getName();
            if (buffName == "Random") {
                int r = rand() % 5;
                if (r == 0) buffName = "Speed";
                else if (r == 1) buffName = "Jump";
                else if (r == 2) buffName = "GoldMagnet";
                else if (r == 3) buffName = "Shield";
                else buffName = "Boom"; 
            }
            p->getRuntimeStatsMutable().storedItemSlot = buffName;
            itemState_ = ItemState::Used;
            std::cout << "[Buff] Collected: " << buffName << " from " << effect_->getName() << "\n";
        } else {
            p->setOverlappingItem(this);
        }
    }
}

void Buff::forceInteract(Entity& other) {
    Player* p = dynamic_cast<Player*>(&other);
    if (p) {
        std::string buffName = effect_->getName();
        if (buffName == "Random") {
            int r = rand() % 5;
            if (r == 0) buffName = "Speed";
            else if (r == 1) buffName = "Jump";
            else if (r == 2) buffName = "GoldMagnet";
            else if (r == 3) buffName = "Shield";
            else buffName = "Boom"; 
        }
        p->getRuntimeStatsMutable().storedItemSlot = buffName;
        itemState_ = ItemState::Used;
        std::cout << "[Buff] Swapped to: " << buffName << "\n";
    }
}
