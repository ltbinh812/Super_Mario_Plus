#pragma once
#include "IEnemySkill.h"
#include <string>

class BasicMeleeEnemySkill : public IEnemySkill {
private:
    std::string animName;
    int damage;
    float hitboxStartTime;
    float hitboxEndTime;
    float duration;

public:
    BasicMeleeEnemySkill(const std::string& anim, int dmg, float startT, float endT, float dur)
        : animName(anim), damage(dmg), hitboxStartTime(startT), hitboxEndTime(endT), duration(dur) {}

    void execute(Mob& mob) override;

    float getDuration() const override { return duration; }
    const std::string& getAnimName() const override { return animName; }
    float getHitboxStartTime() const override { return hitboxStartTime; }
    float getHitboxEndTime() const override { return hitboxEndTime; }
    int getDamage() const override { return damage; }
};
