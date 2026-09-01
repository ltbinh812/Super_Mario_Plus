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

    updateFirstSightTeleport();
}

// =============================================================================
// Dịch chuyển vào trận — CHỈ BOSS, và CHỈ MỘT LẦN.
//
// Boss đứng yên chờ ở phòng của nó. Khoảnh khắc người chơi bước vào bán kính 5
// block quanh boss, boss nhấp nháy tới đứng ngay cạnh người chơi — một màn ra
// mắt, đồng thời khoá luôn khoảng cách để trận đánh bắt đầu ngay thay vì boss
// phải lệt bệt chạy tới (và có thể mắc kẹt ở vách đá giữa đường).
//
// "lần đầu" là đúng nghĩa đen: hasTeleportedOnSight_ không bao giờ được bật
// lại, nên boss không thể đuổi theo người chơi bằng cách nhảy liên tục.
//
// Bán kính tính theo BLOCK chứ không phải pixel, để map vẽ ở tile 16px và map
// vẽ ở tile 32px đều cho ra cùng một khoảng cách cảm nhận được.
// =============================================================================
void Boss::updateFirstSightTeleport() {
    if (hasTeleportedOnSight_) return;
    if (isDead || isWaitingForCutscene) return;

    // Đang trong cutscene ra mắt thì để cảnh quay chạy xong đã.
    if (dynamic_cast<BossIntroState*>(currentState.get())) return;

    const TileMap* map = getMap();
    if (!map) return;

    Player* target = getClosestPlayer();
    if (!target) return;

    // GetTileSize() là kích thước tile trong file nguồn; GetWorldScale() quy
    // đổi sang đơn vị thế giới mà physics đang dùng.
    const float blockPx = map->GetTileSize() * map->GetWorldScale();
    if (blockPx <= 0.0f) return;

    const float sightRadius = kFirstSightBlocks * blockPx;
    if (Vector2Distance(getPosition(), target->getPosition()) > sightRadius) return;

    // Thấy rồi thì coi như đã dùng lượt này, kể cả khi không tìm được ô trống —
    // nếu không, boss sẽ thử lại mỗi frame suốt cả trận.
    hasTeleportedOnSight_ = true;

    if (tryRepositionNear(*target)) {
        isWaitingForCutscene = false;
        changeState(std::make_unique<BossIdleState>());
    }
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

    runtimeStats.health -= damage;

    if (runtimeStats.health <= 0) {
        runtimeStats.health = 0;
        isDead = true;
        changeState(std::make_unique<BossDieState>());
    } else if (forceInterrupt) {
        changeState(std::make_unique<BossHurtState>());
    }
}

