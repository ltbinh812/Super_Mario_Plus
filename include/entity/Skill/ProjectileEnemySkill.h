#pragma once
#include "Skill/IEnemySkill.h"
#include "EntityFactory.h" // EntityType enum
#include <string>

class Mob;

class ProjectileEnemySkill : public IEnemySkill {
private:
    std::string animName;
    int damage;
    float hitboxStartTime;
    float hitboxEndTime;
    float duration;
    EntityType projType;

public:
    ProjectileEnemySkill(const std::string& anim, int dmg, float startT, float endT, float dur, EntityType pType = EntityType::Fireball)
        : animName(anim), damage(dmg), hitboxStartTime(startT), hitboxEndTime(endT), duration(dur), projType(pType) {}

    void execute(Mob& mob) override;

    float getDuration() const override { return duration; }
    const std::string& getAnimName() const override { return animName; }
    float getHitboxStartTime() const override { return hitboxStartTime; }
    float getHitboxEndTime() const override { return hitboxEndTime; }
    int getDamage() const override { return damage; }
};
