#pragma once
#include "GameState.h"
#include "Animation.h"
#include <raylib.h>
#include <memory>
#include <string>

// =============================================================================
// EndgameState — Màn hình kết thúc, dùng chung cho cả hai chế độ chơi.
//
//   1-Player : nền level_completed_background.png (lâu đài — đã qua màn)
//   2-Player : nền winner_background.png (sân khấu trao giải)
//
// Cả hai đều có một nút quay về Main Menu ở GÓC DƯỚI PHẢI, vị trí và kích thước
// tính theo tỉ lệ khung hình nên không lệch khi đổi độ phân giải.
//
// Riêng chế độ 2-Player còn dựng hoạt ảnh của NHÂN VẬT THẮNG đứng trên bục
// podium ở giữa nền, theo đúng cách island trong CharacterSelectionState làm:
// chạy idle lặp vô hạn, thỉnh thoảng chen một lượt animation kỹ năng rồi quay
// lại idle.
//
// LƯU Ý VỀ MỘT LỖI ĐÃ SỬA: trước đây toàn bộ phần nạp tài nguyên nằm trong
// Init(), nhưng GameState không có Init() trong interface và StateManager không
// bao giờ gọi nó -> bgTex_ luôn rỗng, screenW_/screenH_ bằng 0, nút quay về co
// lại thành hình chữ nhật 0x0 không bấm được, màn hình chỉ hiện chữ
// "Game Ended!". Nay Init() được gọi ngay trong constructor giống mọi state
// khác trong dự án.
// =============================================================================
class EndgameState : public GameState {
private:
    bool isPvPMode_;
    std::string winnerName_;   // tên nhân vật thắng; rỗng = hoà hoặc chế độ 1P

    Texture2D bgTex_;
    Texture2D btnNormalTex_;
    Texture2D btnPressTex_;

    Rectangle backBtnRect_;    // vùng VẼ nút (co lại khi không rê chuột)
    Rectangle backBtnHitBox_;  // vùng BẤM, luôn bằng kích thước lúc hover
    bool isBtnHovered_;
    bool isBtnPressed_;
    bool isReturningToMenu_;   // cờ do HandleInput đặt, Process xử lý

    float screenW_;
    float screenH_;

    // --- Hoạt ảnh nhân vật thắng (chỉ dùng ở chế độ 2-Player) ---
    std::unique_ptr<Animation> winnerIdleAnim_;
    std::unique_ptr<Animation> winnerSkillAnim_;
    bool  isPlayingSkill_;
    float skillTimer_;

    // Vị trí bục podium trên nền winner_background.png, theo tỉ lệ màn hình.
    // Nhân vật đứng bằng CHÂN tại điểm này (điểm neo giữa-đáy).
    static constexpr float kPodiumCenterX = 0.50f;
    static constexpr float kPodiumTopY    = 0.672f;
    // Chiều cao nhân vật = 26% chiều cao màn hình (giữ nguyên tỉ lệ khung ảnh).
    static constexpr float kWinnerHeightRatio = 0.26f;

    // Đọc characters.json và nạp animation idle + attack_1 của người thắng.
    void LoadWinnerAnimations();
    // Tính lại backBtnRect_/backBtnHitBox_ theo kích thước màn hình hiện tại.
    void RecalculateLayout();

public:
    // winnerName chỉ có ý nghĩa khi isPvPMode = true.
    EndgameState(bool isPvPMode, const std::string& winnerName = "");
    ~EndgameState() override;

    // Nạp tài nguyên. Đã được constructor gọi sẵn; gọi lại vẫn an toàn vì
    // Init() tự Cleanup() trước.
    void Init();

    void HandleInput() override;
    void Process() override;
    void Update(float dt) override;
    void Render(float alpha) const override;
    void Cleanup();
};
