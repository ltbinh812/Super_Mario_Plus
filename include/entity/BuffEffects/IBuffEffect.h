#pragma once
#include <string>
#include <memory>

class Player;

class IBuffEffect {
public:
    virtual ~IBuffEffect() = default;

    virtual std::string getName() const = 0;
    virtual std::string getFrameName() const = 0;
    virtual int getFrameCount() const { return 1; }
    virtual float getFrameTime() const { return 0.1f; }
    virtual float getDuration() const = 0;

    // Multipliers for stats (return 0.0f for no effect, e.g. 0.5f means +50%)
    virtual float getSpeedMultiplier() const { return 0.0f; }
    virtual float getJumpMultiplier() const { return 0.0f; }
    virtual float getDamageMultiplier() const { return 0.0f; }
    
    // Flags
    virtual bool isInvincible() const { return false; }
    virtual bool canTimeStop() const { return false; }
    virtual bool hasGoldMagnet() const { return false; }

    // Cloning for BuffManager
    virtual std::unique_ptr<IBuffEffect> clone() const = 0;

    // Optional hooks for entering/exiting buff state
    virtual void onApply(Player& player) {}
    virtual void onRemove(Player& player) {}
};
