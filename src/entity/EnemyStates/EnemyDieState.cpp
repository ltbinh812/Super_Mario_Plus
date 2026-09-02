#include "EnemyStates/EnemyDieState.h"
#include "Mob.h"
#include "CommandQueue.h"

// =============================================================================
// Quai thuong roi COIN khi chet.
//
// Dat trong enter() chu khong phai process(): enter() chay dung MOT lan luc vao
// trang thai, con process() chay moi frame — de o process() thi mot con quai se
// no ra hang tram dong coin trong luc dien animation chet.
//
// Khong tao Coin truc tiep tai day. Trang thai nay dang duoc goi TU BEN TRONG
// vong lap duyet danh sach entity cua man choi; chen thang vao danh sach do la
// hong iterator. Thay vao do day mot SpawnCommand vao hang doi, va BaseLevelState
// se dung no o pha Process — luc khong con ai dang duyet danh sach.
// =============================================================================
void EnemyDieState::enter(Mob& mob) {
    mob.setAnimation(mob.getMobType() + "_die");
    mob.setVelocity({ 0.0f, 0.0f });
    mob.setHitboxActive(false);

    mob.dropLootOnce("Coin");   // ĐÚNG MỘT đồng xu cho mỗi con quái
}

void EnemyDieState::decideAction(Mob& mob) {
    // Dead mobs make no decisions
}

void EnemyDieState::process(Mob& mob) {
    // Waiting for animation to finish is handled by Mob::getIsActive()
}

void EnemyDieState::exit(Mob& mob) {
}
