#include "BasicMeleeEnemySkill.h"
#include "Mob.h"
#include "Player.h"
#include <iostream>
#include <raymath.h>

void BasicMeleeEnemySkill::execute(Mob& mob) {
    Player* p = mob.getClosestPlayer();
    if (p && !p->isDead()) {
        bool hit = false;
        
        // Nếu skill có khai báo box trong JSON (offsetX, offsetY, width, height)
        if (box.width > 0 && box.height > 0) {
            Rectangle hitBox;
            hitBox.width = box.width;
            hitBox.height = box.height;
            float offX = mob.getIsFacingRight() ? box.x : (-box.x - hitBox.width);
            hitBox.x = mob.getPosition().x + offX;
            hitBox.y = mob.getPosition().y + box.y;
            
            if (CheckCollisionRecs(hitBox, p->getHitbox())) {
                hit = true;
            }
        } 
        // Fallback: Dùng khoảng cách (attackRange)
        else {
            float dist = Vector2Distance(mob.getPosition(), p->getPosition());
            if (dist <= mob.getConfig().aiData.attackRange) {
                hit = true;
            }
        }
        
        if (hit) {
            float knockbackDir = (p->getPosition().x > mob.getPosition().x) ? 1.0f : -1.0f;
            p->takeDamage(damage, knockbackDir, true);
        }
    }
}
