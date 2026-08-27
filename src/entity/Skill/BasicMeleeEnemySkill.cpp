#include "BasicMeleeEnemySkill.h"
#include "Mob.h"
#include "Player.h"
#include <iostream>
#include <raymath.h>

void BasicMeleeEnemySkill::execute(Mob& mob) {
    // Basic melee attack logic
    // We get the closest player and check if they are in range
    Player* p = mob.getClosestPlayer();
    if (p && !p->isDead()) {
        float dist = Vector2Distance(mob.getPosition(), p->getPosition());
        
        // Use mob's attack range from config
        if (dist <= mob.getConfig().aiData.attackRange) {
            // Apply damage
            // In a real game, you would also use CombatSystem or Hitboxes,
            // but for a simple melee attack, distance check is enough.
            float knockbackDir = (p->getPosition().x > mob.getPosition().x) ? 1.0f : -1.0f;
            p->takeDamage(damage, knockbackDir, true);
        }
    }
}
