#pragma once
#include "IMobState.h"

// =============================================================================
// BossIntroState — màn ra mắt của boss.
//
// HAI ĐƯỜNG VÀO, hai điều kiện kết thúc khác nhau:
//
//   waitForCutscene = true   (mặc định, dùng khi cutscene kích hoạt)
//       Chờ CẢ hai: cutscene báo xong VÀ animation intro chạy hết.
//
//   waitForCutscene = false  (dùng khi boss lần đầu thấy người chơi)
//       Chỉ chờ animation intro chạy hết. Không có cutscene nào để đợi.
//
// Vì sao cần đường thứ hai: BossIntroState trước đây CHỈ vào được từ
// Boss::onCutsceneStart(), tức boss bắt buộc phải có cutsceneId khớp với một
// CutsceneTrigger trong map. Nhưng boss trong các file .ldtk đều để
// cutsceneId = null, nên trạng thái này không bao giờ chạy — boss không hề có
// màn ra mắt dù cả 6 con đều có sẵn animation intro 7–30 khung.
// =============================================================================
class BossIntroState : public IMobState {
private:
    bool  isCutsceneFinished = false;
    bool  waitForCutscene_   = true;
    float elapsed_           = 0.0f;

    // Sàn thời gian: nếu animation intro thiếu hoặc quá ngắn thì màn ra mắt
    // vẫn kịp nhìn thấy, thay vì loé một khung rồi biến mất.
    static constexpr float kMinDuration = 0.6f;

    // Trần thời gian: không để boss đứng hình vĩnh viễn nếu animation intro
    // được cấu hình lặp (isFinished() sẽ không bao giờ đúng).
    static constexpr float kMaxDuration = 6.0f;

public:
    explicit BossIntroState(bool waitForCutscene = true)
        : waitForCutscene_(waitForCutscene) {}

    void enter(Mob& mob) override;
    void decideAction(Mob& mob) override;
    void process(Mob& mob) override;
    void exit(Mob& mob) override;

    void markCutsceneFinished() { isCutsceneFinished = true; }
};
