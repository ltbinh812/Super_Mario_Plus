#include "ProjectileEnemySkill.h"
#include "Mob.h"
#include "EntityFactory.h" // EntityType enum

void ProjectileEnemySkill::execute(Mob& mob) {
    auto* cq = mob.getCommandQueue();
    if (!cq) return;

    SpawnCommand cmd;
    cmd.category = SpawnCategory::Entity;
    cmd.type = projType; 
    
    // EntityFactory đã tự động cộng thêm offsetX và offsetY từ cấu hình fireball trong JSON
    cmd.position = mob.getPosition();
    
    cmd.isFacingRight = mob.getIsFacingRight();
    cmd.ownerName = mob.getMobType(); // Truyền tên Boss (VD: boss_chopper) để Factory đọc đúng config "special_ball" trong file JSON
    cmd.spawner = &mob; // Để đạn biết ai bắn ra (tránh gây sát thương lên người bắn)
    
    cq->push(cmd);
}
