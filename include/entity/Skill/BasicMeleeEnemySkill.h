#pragma once
#include "Skill/IEnemySkill.h"
#include <string>
#include <raylib.h>

class BasicMeleeEnemySkill : public IEnemySkill {
private:
    std::string animName;
    int damage;
    float hitboxStartTime;
    float hitboxEndTime;
    float duration;
    Rectangle box;

public:
    BasicMeleeEnemySkill(const std::string& anim, int dmg, float startT, float endT, float dur, Rectangle b = {0,0,0,0})
        : animName(anim), damage(dmg), hitboxStartTime(startT), hitboxEndTime(endT), duration(dur), box(b) {}

    void execute(Mob& mob) override;

    bool emitsHitbox() const override { return true; }
    Hitbox getHitbox(Mob& mob) const override;

    float getDuration() const override { return duration; }
    const std::string& getAnimName() const override { return animName; }
    float getHitboxStartTime() const override { return hitboxStartTime; }
    float getHitboxEndTime() const override { return hitboxEndTime; }
    int getDamage() const override { return damage; }
};
