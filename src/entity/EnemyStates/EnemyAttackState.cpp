#include "EnemyStates/EnemyAttackState.h"
#include "EnemyStates/EnemyIdleState.h"
#include "Mob.h"
#include "Hitbox.h"

EnemyAttackState::EnemyAttackState()
    : windupTime(0), activeTime(0), recoveryTime(0), timer(0.0f), hasAttacked(false), damage(0) {}

void EnemyAttackState::enter(Mob& mob) {
    mob.setAnimation(mob.getMobType() + "_attack");
    mob.setVelocity({0.0f, mob.getVelocity().y});
    
    auto& atkData = mob.getConfig().attackData;
    windupTime = atkData.hitboxStartFrame * atkData.frameTime;
    activeTime = (atkData.hitboxEndFrame - atkData.hitboxStartFrame) * atkData.frameTime;
    recoveryTime = (atkData.hitboxTotalFrames - atkData.hitboxEndFrame) * atkData.frameTime;
    damage = atkData.damage;
}

void EnemyAttackState::decideAction(Mob& mob) {
    if (timer >= windupTime + activeTime + recoveryTime) {
        mob.setAttackCooldown(1.5f);
        mob.changeState(std::make_unique<EnemyIdleState>());
    }
}

void EnemyAttackState::process(Mob& mob) {
    timer += GetFrameTime();
    
    if (timer >= windupTime && timer < windupTime + activeTime) {
        if (!hasAttacked) {
            hasAttacked = true;
            // Activate hitbox
            Hitbox hb;
            hb.damage = damage;
            hb.owner = &mob;
            
            auto& boxData = mob.getConfig().attackData.box;
            float w = boxData.width;
            float h = boxData.height;
            float offsetX = boxData.x;
            float offsetY = boxData.y;
            
            float x = mob.getPosition().x + (mob.getIsFacingRight() ? offsetX : -offsetX - w);
            float y = mob.getPosition().y - h / 2.0f + offsetY;
            hb.rect = {x, y, w, h};
            hb.targetFactionMask = (1 << static_cast<int>(EntityFaction::Player));
            
            mob.setHitboxActive(true, hb);
        }
    } else {
        mob.setHitboxActive(false);
    }
}

void EnemyAttackState::exit(Mob& mob) {
    mob.setHitboxActive(false);
}
