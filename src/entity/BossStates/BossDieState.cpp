#include "BossStates/BossDieState.h"
#include "Mob.h"
#include "CommandQueue.h"
#include <algorithm>
#include <cctype>

BossDieState::BossDieState() : dieTimer(0.0f) {}

// =============================================================================
// Boss LUON roi ra dung MOT Key khi chet.
//
// Key la thu mo cua/cong sang khu tiep theo, nen day chinh la phan thuong gan
// tien do man choi vao viec ha boss. "Luon luon" nghia la khong random: nguoi
// choi thang boss thi chac chan di tiep duoc.
//
// Cung ly do voi EnemyDieState: dat o enter() de chi chay mot lan, va day qua
// hang doi spawn thay vi tu tao entity giua luc man choi dang duyet danh sach.
// =============================================================================
void BossDieState::enter(Mob& mob) {
    std::string lowerType = mob.getMobType();
    std::transform(lowerType.begin(), lowerType.end(), lowerType.begin(), ::tolower);
    mob.setAnimation(lowerType + "_die");
    mob.setVelocity({0.0f, mob.getVelocity().y});
    mob.setHitboxActive(false);

    mob.dropLootOnce("Key");    // ĐÚNG MỘT chìa khoá cho mỗi boss
}

void BossDieState::decideAction(Mob& mob) {
    // Boss đã chết, không làm gì cả
}

void BossDieState::process(Mob& mob) {
    dieTimer += GetFrameTime();
}

void BossDieState::exit(Mob& mob) {
}
