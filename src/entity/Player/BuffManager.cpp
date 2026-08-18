#include "BuffManager.h"
#include <algorithm>

void BuffManager::addBuff(std::unique_ptr<IBuffEffect> buff, Player& player) {
    if (!buff) return;
    
    // Check if we already have this buff, just refresh duration
    for (auto& active : activeBuffs_) {
        if (active.effect->getName() == buff->getName()) {
            active.remainingTime = buff->getDuration();
            return;
        }
    }
    
    // Otherwise add new buff
    float duration = buff->getDuration();
    buff->onApply(player);
    activeBuffs_.push_back({std::move(buff), duration});
}

void BuffManager::update(float dt, Player& player) {
    for (auto it = activeBuffs_.begin(); it != activeBuffs_.end(); ) {
        it->remainingTime -= dt;
        if (it->remainingTime <= 0.0f) {
            it->effect->onRemove(player);
            it = activeBuffs_.erase(it);
        } else {
            ++it;
        }
    }
}

void BuffManager::clear(Player& player) {
    for (auto& active : activeBuffs_) {
        active.effect->onRemove(player);
    }
    activeBuffs_.clear();
}

float BuffManager::getTotalSpeedMultiplier() const {
    float total = 0.0f;
    for (const auto& active : activeBuffs_) {
        total += active.effect->getSpeedMultiplier();
    }
    return total;
}

float BuffManager::getTotalJumpMultiplier() const {
    float total = 0.0f;
    for (const auto& active : activeBuffs_) {
        total += active.effect->getJumpMultiplier();
    }
    return total;
}

float BuffManager::getTotalDamageMultiplier() const {
    float total = 0.0f;
    for (const auto& active : activeBuffs_) {
        total += active.effect->getDamageMultiplier();
    }
    return total;
}

bool BuffManager::isInvincible() const {
    for (const auto& active : activeBuffs_) {
        if (active.effect->isInvincible()) return true;
    }
    return false;
}

bool BuffManager::canTimeStop() const {
    for (const auto& active : activeBuffs_) {
        if (active.effect->canTimeStop()) return true;
    }
    return false;
}

bool BuffManager::hasGoldMagnet() const {
    for (const auto& active : activeBuffs_) {
        if (active.effect->hasGoldMagnet()) return true;
    }
    return false;
}
