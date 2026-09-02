#include "Boss.h"
#include "Player.h"
#include "TileMap.h"
#include "BossStates/BossIdleState.h"
#include "EnemyStates/EnemyRunState.h"
#include "BossStates/BossIntroState.h"
#include "BossStates/BossHurtState.h"
#include "BossStates/BossDieState.h"
#include <iostream>

Boss::Boss(Vector2 worldPos, const std::string& mobType, const CharacterBaseStats& bStats, const MobConfig& config, const std::string& cutsceneId)
    : Mob(worldPos, mobType, bStats, config), cutsceneId(cutsceneId) {
    
    isWaitingForCutscene = !this->cutsceneId.empty();
    
    // Fail-safe initialization based on cutsceneId
    if (this->cutsceneId.empty()) {
        // std::cout << "[Boss] No cutsceneId provided for Boss. Skipping Intro and defaulting to Idle State.\n";
        changeState(std::make_unique<BossIdleState>());
    } else {
        // std::cout << "[Boss] Initialized with cutsceneId: " << this->cutsceneId << ". Waiting for Intro.\n";
        changeState(std::make_unique<BossIdleState>());
    }
}

#include "BossStates/BossDebugInputState.h"
#include <raylib.h>
#include <raymath.h>

void Boss::update(float dt) {
    Mob::update(dt);

    // ĐÃ GỠ: công tắc debug bằng phím P.
    //
    // Ba lỗi trong một đoạn code:
    //   1. Nó nằm trong update(), tức pha Update chạy theo bước cố định — có
    //      frame chạy 0 lần, có frame chạy 2 lần. Đọc bàn phím ở đây vi phạm
    //      quy tắc 4 pha và có thể bật/tắt hai lần trong một lần bấm phím.
    //   2. MỌI boss trong màn cùng nghe một phím, nên bấm P là cả đám nhảy vào
    //      BossDebugInputState một lượt.
    //   3. Nó còn nguyên trong bản giao nộp: người chơi vô tình chạm P giữa
    //      trận là boss đứng im chờ điều khiển tay.
    //
    // BossDebugInputState vẫn được giữ lại — vào được bằng cách gọi
    // enterDebugMode() từ code khi cần chỉnh timing hitbox.

    // Cứu hộ cho trường hợp cutscene không bao giờ nổ.
    //
    // isWaitingForCutscene chỉ được tắt trong onCutsceneStart(). Nếu vùng
    // trigger trong file .ldtk bị thiếu hoặc ghi sai id, cờ này không bao giờ
    // hạ, mà BossIdleState::decideAction/process đều return sớm khi nó còn bật
    // -> boss đứng như tượng suốt màn, không đánh mà cũng không đuổi.
    // Người chơi tới sát tận nơi thì coi như trận đã bắt đầu, cho boss vào cuộc.
    if (isWaitingForCutscene) {
        if (Player* target = getClosestPlayer()) {
            float dist = Vector2Distance(getPosition(), target->getPosition());
            if (dist <= config.aiData.attackRange * 1.5f) {
                isWaitingForCutscene = false;
                changeState(std::make_unique<BossIdleState>());
            }
        }
    }

    updateTeleport(dt);
}

// =============================================================================
// Dịch chuyển của boss — HAI giai đoạn, hai điều kiện khác nhau.
//
//   LẦN ĐẦU  : người chơi lọt vào bán kính 5 BLOCK -> nhảy ngay. Đây là cú ra
//              mắt: boss không lệt bệt chạy tới (và mắc kẹt ở vách đá giữa
//              đường), nó xuất hiện ngay cạnh và trận đánh bắt đầu.
//
//   LẦN SAU  : phải trôi qua 5~10 giây mà boss KHÔNG ăn đòn nào của người chơi.
//              Đây là cơ chế chống bỏ chạy: áp sát đánh liên tục thì boss đứng
//              yên đánh trả; bỏ chạy, nấp sau địa hình hay bắn hụt suốt thì
//              boss tự tìm tới.
//
// Vì sao mốc là "bị đánh" chứ không phải khoảng cách: đứng gần vẫn có thể
// không trúng phát nào, nên khoảng cách không phân biệt được "đang đánh nhau"
// với "đang né". Sát thương thì phân biệt được.
//
// Sát thương môi trường KHÔNG tính (xem onDamagedBy) — đứng trong dung nham
// không phải là đang đánh boss.
//
// Ngưỡng bốc ngẫu nhiên trong [5,10] giây để boss không nhảy theo nhịp đều
// như máy đếm, và người chơi không học thuộc được thời điểm.
//
// Bán kính tính theo BLOCK chứ không phải pixel, để map vẽ ở tile 16px và map
// vẽ ở tile 32px cho cùng một khoảng cách cảm nhận được.
// =============================================================================
void Boss::updateTeleport(float dt) {
    if (isDead || isWaitingForCutscene) return;

    // Đang diễn cảnh ra mắt thì để cảnh quay chạy xong đã.
    if (dynamic_cast<BossIntroState*>(currentState.get())) return;

    Player* target = getClosestPlayer();
    if (!target || target->isDead()) return;

    // ---- Thoát khỏi môi trường nguy hiểm: Poison / Lava ------------------
    //
    // Không gắn với hasFirstTeleport_: boss cần thoát lava kể cả trước khi
    // trận bắt đầu (tránh boss chết âm thầm trong dung nham khi player chưa
    // tiếp cận). Đây là cơ chế survival độc lập với hệ thống teleport chiến đấu.
    {
        auto liq = getRuntimeStats().currentLiquid;
        bool inDanger = (liq == CollisionType::Poison || liq == CollisionType::Lava);
        if (inDanger) {
            liquidDangerTimer_ += dt;
            if (liquidDangerTimer_ >= kLiquidDangerLimit) {
                liquidDangerTimer_ = 0.0f;
                // Reset bộ đếm stall luôn để tránh teleport kép ngay sau đó.
                armNextStall();
                tryRepositionNear(*target);
                return; // Đã xử lý teleport lần này, bỏ qua logic stall bên dưới.
            }
        } else {
            liquidDangerTimer_ = 0.0f; // Ra khỏi liquid: reset bộ đếm
        }
    }

    // ---- Lần đầu: chờ người chơi vào tầm 5 block ----------------------------
    if (!hasFirstTeleport_) {
        const TileMap* map = getMap();
        if (!map) return;                       // không có map thì không dám nhảy
        const float blockPx = map->GetTileSize() * map->GetWorldScale();
        if (blockPx <= 0.0f) return;

        const float sightRadius = kFirstSightBlocks * blockPx;
        if (Vector2Distance(getPosition(), target->getPosition()) > sightRadius) return;

        // Đánh dấu đã dùng lượt ra mắt kể cả khi không tìm được ô trống, để
        // boss không thử lại mỗi khung hình.
        hasFirstTeleport_ = true;
        armNextStall();
        tryRepositionNear(*target);
        return;
    }

    // ---- Những lần sau: 5~10 giây không ăn đòn của người chơi ---------------
    noDamageTimer_ += dt;
    if (noDamageTimer_ < nextStallDelay_) return;

    armNextStall();          // đặt lại NGAY, kể cả khi không tìm được chỗ trống
    tryRepositionNear(*target);
}


void Boss::armNextStall() {
    noDamageTimer_ = 0.0f;
    const float span = kStallMax - kStallMin;
    nextStallDelay_ = kStallMin + (float)GetRandomValue(0, 1000) / 1000.0f * span;
}

// Chỉ CombatSystem gọi hàm này, và chỉ khi có hitbox thật gây sát thương — nên
// lava/độc (gọi thẳng takeDamage từ Effects.cpp) không bao giờ tới được đây.
// Thêm một lớp lọc theo phe để đòn của quái khác cũng không tính.
void Boss::onDamagedBy(Entity* attacker, int amount) {
    (void)amount;
    if (!attacker) return;
    if (attacker->getFaction() != EntityFaction::Player) return;
    noDamageTimer_ = 0.0f;
}

void Boss::enterDebugMode(bool enable) {
    isDebugMode = enable;
    if (enable) {
        changeState(std::make_unique<BossDebugInputState>());
    } else {
        changeState(std::make_unique<BossIdleState>());
    }
}

void Boss::onCutsceneStart(const std::string& triggerId) {
    if (!cutsceneId.empty() && cutsceneId == triggerId) {
        // std::cout << "[Boss] Cutscene started matching my ID (" << cutsceneId << "). Entering Intro State.\n";
        isDebugMode = false; // Tắt debug nếu cutscene bắt đầu đè lên
        isWaitingForCutscene = false;
        changeState(std::make_unique<BossIntroState>());
    }
}

void Boss::onCutsceneEnd(const std::string& triggerId) {
    if (!cutsceneId.empty() && cutsceneId == triggerId) {
        // std::cout << "[Boss] Cutscene ended matching my ID (" << cutsceneId << "). Notifying Intro State.\n";
        
        // Notify the current state (if it's BossIntroState) that cutscene is finished
        // We can do this by using dynamic_cast
        if (auto* introState = dynamic_cast<BossIntroState*>(currentState.get())) {
            introState->markCutsceneFinished();
        }
    }
}

void Boss::initAnimations(int attackFrames, int runFrames, int idleFrames, int hurtFrames, int dieFrames, int introFrames) {
    std::string lowerType = mobType;
    std::transform(lowerType.begin(), lowerType.end(), lowerType.begin(), ::tolower);
    
    animations[lowerType + "_attack"] = AtlasAnimation(lowerType + "_attack", attackFrames, 0.1f, true);
    animations[lowerType + "_run"] = AtlasAnimation(lowerType + "_run", runFrames, 0.1f, true);
    animations[lowerType + "_idle"] = AtlasAnimation(lowerType + "_idle", idleFrames, 0.1f, true);
    animations[lowerType + "_hurt"] = AtlasAnimation(lowerType + "_hurt", hurtFrames, 0.1f, false);
    animations[lowerType + "_die"] = AtlasAnimation(lowerType + "_die", dieFrames, 0.1f, false);
    
    if (introFrames > 0) {
        animations[lowerType + "_intro"] = AtlasAnimation(lowerType + "_intro", introFrames, 0.1f, false);
    }
    
    setAnimation(lowerType + "_idle");
}

void Boss::takeDamage(int damage, float knockbackDirX, bool forceInterrupt) {
    if (isDead || hurtTimer > 0.0f) return;

    // Boss còn đang chờ cutscene thì chưa vào trận: nó đứng bất động và không
    // phản đòn được. Cho phép bắn tỉa lúc này là để người chơi hạ boss trước
    // khi trận bắt đầu — nhận đòn xong vẫn đứng im vì BossIdleState còn bị khoá.
    // Thay vào đó, cú đánh đầu tiên chính là tín hiệu vào trận.
    if (isWaitingForCutscene) {
        isWaitingForCutscene = false;
        changeState(std::make_unique<BossIdleState>());
    }

    // CỐ Ý KHÔNG đặt lại noDamageTimer_ ở đây.
    //
    // takeDamage() không biết ai đánh, mà Effects.cpp gọi thẳng nó cho đầm độc
    // và dung nham. Đặt lại ở đây thì boss đứng trong lava sẽ tưởng người chơi
    // đang đánh mình và không bao giờ dịch chuyển nữa.
    // Việc đó thuộc về onDamagedBy(), nơi biết rõ kẻ tấn công là ai.

    runtimeStats.health -= damage;

    if (runtimeStats.health <= 0) {
        runtimeStats.health = 0;
        isDead = true;
        changeState(std::make_unique<BossDieState>());
    } else if (forceInterrupt) {
        changeState(std::make_unique<BossHurtState>());
    }
}

