#pragma once
#include "IMenuPanel.h"
#include "PanelButton.h"
#include "raylib.h"
#include <functional>
#include <string>

// =============================================================================
// WorldActionPanel — Hộp thoại hiện đè lên MapSelectionState ngay sau khi người
// chơi bấm vào một world.
//
//        +---------------------------+
//        |         WORLD 03          |
//        |    [    NEW GAME     ]    |
//        |    [   LOAD GAME     ]    |
//        |    [      BACK       ]    |
//        +---------------------------+
//
// VÌ SAO LÀ PANEL CHỨ KHÔNG PHẢI MỘT GameState MỚI?
// Người chơi vẫn đang "ở màn chọn map", chỉ là được hỏi thêm một câu. Dựng hẳn
// một GameState sẽ kéo theo một lần chuyển cảnh (iris transition), phải nạp lại
// toàn bộ texture nền và 6 building — tốn kém cho một câu hỏi hai lựa chọn.
// Đây đúng khuôn mẫu IngameSettingsPanel/ShopUIPanel mà dự án đang dùng.
//
// TUÂN THỦ 4 GIAI ĐOẠN:
//   HandleInput() : chỉ đẩy toạ độ chuột xuống các PanelButton
//   Update(dt)    : tính lại layout/hitbox + chạy animation mở panel
//   Render()      : chỉ vẽ, không đổi một biến nào
// Sự kiện click được panel lấy ra bằng PanelButton::ConsumeClick() trong
// Update() rồi gọi callback tương ứng.
//
// SỞ HỮU TÀI NGUYÊN: panel tự Load/Unload texture của riêng nó. Font thì KHÔNG:
// font được MapSelectionState mượn cho, nếu panel unload sẽ thành double-free.
// =============================================================================
class WorldActionPanel : public IMenuPanel {
private:
    bool isOpen_ = false;
    int  worldIndex_ = -1;
    std::string worldName_ = "";

    float screenW_ = 0.0f;
    float screenH_ = 0.0f;

    // Texture của riêng panel (được Unload trong destructor)
    Texture2D barTex_ = {0};
    Texture2D barPressTex_ = {0};

    Font font_ = {0};      // MƯỢN từ MapSelectionState — tuyệt đối không Unload
    bool hasFont_ = false;

    Rectangle panelRect_ = {0, 0, 0, 0};

    PanelButton newGameBtn_;
    PanelButton loadGameBtn_;
    PanelButton backBtn_;

    // Animation mở panel: 0 -> 1 trong ~0.22s, dùng để trượt lên + mờ dần vào
    float animT_ = 0.0f;

    std::function<void()> onNewGame_;
    std::function<void()> onLoadGame_;
    std::function<void()> onClose_;

    // Tính lại panelRect_ và bounds của 3 nút. Gọi trong Update() — KHÔNG được
    // gọi trong Render() vì đó là thay đổi trạng thái.
    void RecalculateLayout();

public:
    WorldActionPanel();
    ~WorldActionPanel() override;

    // Nạp texture, ghi nhớ kích thước màn hình và font mượn.
    void Init(float screenWidth, float screenHeight, Font font);

    void Open(int worldIndex, const std::string& worldName);
    void Close();
    bool IsOpen() const { return isOpen_; }
    int  GetWorldIndex() const { return worldIndex_; }

    void SetOnNewGame(std::function<void()> callback) { onNewGame_ = std::move(callback); }
    void SetOnLoadGame(std::function<void()> callback) { onLoadGame_ = std::move(callback); }
    void SetOnCloseCallback(std::function<void()> onClose) override { onClose_ = std::move(onClose); }

    void HandleInput(Vector2 mousePos, bool mousePressed, bool mouseReleased) override;
    void Update(float dt) override;
    void Render() const override;
};
