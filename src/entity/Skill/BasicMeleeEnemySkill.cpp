#include "BasicMeleeEnemySkill.h"
#include "Mob.h"
#include "Player.h"
#include <iostream>
#include <raymath.h>

void BasicMeleeEnemySkill::execute(Mob& mob) {
    // Execution is now handled by the continuous hitbox emitted to the CombatSystem
    // We can leave this empty or use it for one-time effects (like sound or particles)
}

Hitbox BasicMeleeEnemySkill::getHitbox(Mob& mob) const {
    Rectangle hitBox = {0, 0, 0, 0};
    
    // Nếu skill có khai báo box trong JSON (offsetX, offsetY, width, height)
    if (box.width > 0 && box.height > 0) {
        hitBox.width = box.width;
        hitBox.height = box.height;
        float offX = mob.getIsFacingRight() ? box.x : (-box.x - hitBox.width);
        hitBox.x = mob.getPosition().x + offX;
        hitBox.y = mob.getPosition().y + box.y;
    } 
    // Fallback: Dùng khoảng cách (attackRange)
    else {
        float range = mob.getConfig().aiData.attackRange;
        hitBox.width = range;
        hitBox.height = mob.getHitbox().height;
        float offX = mob.getIsFacingRight() ? 0 : -range;
        hitBox.x = mob.getPosition().x + offX;
        hitBox.y = mob.getPosition().y - hitBox.height;
    }
    
    Hitbox hb(hitBox, damage, 0, &mob);
    hb.targetFactionMask = (1 << static_cast<int>(EntityFaction::Player));
    return hb;
}
