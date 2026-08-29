#pragma once
#include <string>
#include "combatsystem/Hitbox.h"

class Mob;

class IEnemySkill {
public:
    virtual ~IEnemySkill() = default;

    // Executes the skill logic on the Mob
    virtual void execute(Mob& mob) = 0;

    // Indicates whether this skill emits a continuous hitbox over its duration window
    virtual bool emitsHitbox() const { return false; }
    
    // Gets the current hitbox for this skill
    virtual Hitbox getHitbox(Mob& mob) const { return Hitbox(); }

    // Time (in seconds) the skill takes to complete the entire animation
    virtual float getDuration() const = 0;

    // Name of the animation string associated with this skill (e.g. "attack1")
    virtual const std::string& getAnimName() const = 0;

    // Timing for when the hitbox becomes active and inactive
    virtual float getHitboxStartTime() const = 0;
    virtual float getHitboxEndTime() const = 0;

    // Damage value
    virtual int getDamage() const = 0;
};
