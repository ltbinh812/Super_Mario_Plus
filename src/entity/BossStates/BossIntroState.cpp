#include "BossIntroState.h"
#include "Boss.h"
#include "BossStates/BossIdleState.h"
#include <raylib.h>

void BossIntroState::enter(Mob& mob) {
    // Nếu enemies.json không khai báo "intro" thì setAnimation sẽ giữ nguyên
    // animation cũ; trần kMaxDuration bên dưới lo phần thoát.
    mob.setAnimation("intro");
    mob.setVelocity({0, 0});
    isCutsceneFinished = false;
    elapsed_ = 0.0f;
}

void BossIntroState::decideAction(Mob& mob) {
    // Cố ý để trống: boss đứng yên diễn, không quyết định hành động nào.
}

void BossIntroState::process(Mob& mob) {
    // process() chạy trong pha Process — mỗi khung hình đúng một lần — nên
    // GetFrameTime() là bước thời gian đúng ở đây. (update(dt) mới là pha bước
    // cố định, nhưng IMobState không có hàm nào nhận dt.)
    elapsed_ += GetFrameTime();

    bool animFinished = false;
    if (mob.hasStandardAnimations() && mob.getCurrentStandardAnim()) {
        if (mob.getCurrentStandardAnim()->isFinished()) {
            animFinished = true;
        } else if (mob.getCurrentStandardAnim()->isLooping()) {
            animFinished = true;
        }
    } else if (mob.getCurrentAnim()) {
        if (mob.getCurrentAnim()->isFinished()) {
            animFinished = true;
        } else if (mob.getCurrentAnim()->isLooping()) {
            // Animation intro lặp (hoặc boss không có intro nên rơi về idle) thì
            // isFinished() không bao giờ đúng — coi như xong ngay.
            animFinished = true;
        }
    }

    // Đứng hình ở khung cuối trông như treo game, nên chuyển sang idle ngay khi
    // intro chạy hết, kể cả lúc còn phải đợi cutscene.
    if (animFinished) {
        mob.setAnimation("idle");
    }

    // Sàn thời gian chỉ áp cho đường "lần đầu thấy người chơi": animation intro
    // của một số boss chỉ 7 khung × 0.1s = 0.7s, và nếu boss nào đó thiếu hẳn
    // intro thì animFinished đúng ngay khung đầu -> màn ra mắt biến mất.
    // Đường cutscene không cần sàn vì đã có cutscene giữ nhịp.
    if (!waitForCutscene_) {
        if (animFinished && elapsed_ >= kMinDuration) {
            mob.changeState(std::make_unique<BossIdleState>());
        } else if (elapsed_ >= kMaxDuration) {
            mob.changeState(std::make_unique<BossIdleState>());
        }
        return;
    }

    // Đường cutscene: chờ CẢ hai mới vào trận, để boss không nhảy vào đánh giữa
    // lúc camera còn đang quay cảnh.
    if (isCutsceneFinished && animFinished) {
        mob.changeState(std::make_unique<BossIdleState>());
    } else if (elapsed_ >= kMaxDuration) {
        // Cứu hộ: cutscene không bao giờ báo kết thúc (trigger sai id, người
        // chơi chết giữa cảnh...) thì vẫn phải trả boss về trạng thái chơi được.
        mob.changeState(std::make_unique<BossIdleState>());
    }
}

void BossIntroState::exit(Mob& mob) {
}
