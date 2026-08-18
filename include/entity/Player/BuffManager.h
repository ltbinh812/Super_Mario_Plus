#pragma once
#include <vector>
#include <memory>
#include "IBuffEffect.h"

class Player;

class BuffManager {
public:
    BuffManager() = default;
    
    void addBuff(std::unique_ptr<IBuffEffect> buff, Player& player);
    void update(float dt, Player& player);
    void clear(Player& player);

    // Multipliers (sums all active buffs)
    float getTotalSpeedMultiplier() const;
    float getTotalJumpMultiplier() const;
    float getTotalDamageMultiplier() const;

    // Flags (true if any active buff provides it)
    bool isInvincible() const;
    bool canTimeStop() const;
    bool hasGoldMagnet() const;

    struct ActiveBuff {
        std::unique_ptr<IBuffEffect> effect;
        float remainingTime;
    };
    
    const std::vector<ActiveBuff>& getActiveBuffs() const { return activeBuffs_; }

private:
    std::vector<ActiveBuff> activeBuffs_;
};
