#include "ExplosionEnemySkill.h"
#include "Mob.h"
#include "EntityFactory.h" // EntityType enum

void ExplosionEnemySkill::execute(Mob& mob) {
    auto* cq = mob.getCommandQueue();
    if (!cq) return;

    SpawnCommand cmd;
    cmd.category = SpawnCategory::Entity;
    cmd.type = EntityType::Explosion;
    
    // EntityFactory đã tự động cộng thêm offsetX và offsetY từ cấu hình explosion trong JSON
    cmd.position = mob.getPosition();
    
    cmd.isFacingRight = mob.getIsFacingRight();
    cmd.ownerName = mob.getMobType(); // Truyền tên Boss (VD: boss_chopper) để Factory đọc đúng config "explosion" trong file JSON
    cmd.spawner = &mob;
    
    cq->push(cmd);
}
