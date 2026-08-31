#pragma once
#include "IMenuPanel.h"
#include "PanelButton.h"
#include "SaveSlotInfo.h"
#include "raylib.h"
#include <functional>
#include <string>
#include <vector>

// =============================================================================
// SaveVersionPanel — Danh sách các bản lưu của MỘT world, hiện đè lên
// MapSelectionState sau khi bấm LOAD GAME.
//
//   +--------------------------------------------------+
//   |             LOAD GAME - WORLD 03                 |
//   |  > v3   Goku    World_Level_4   128c   240/300   |
//   |    v2   Goku    Top              96c   180/300   |
//   |    v1   Naruto  Entrance          12c  280/280   |
//   |                                                  |
//   |            [  DELETE  ]   [  LOAD  ]             |
//   +--------------------------------------------------+
//
// LUỒNG TƯƠNG TÁC (đúng yêu cầu người dùng):
//   1. Mở panel -> danh sách version, mới nhất trên cùng, CHƯA chọn gì.
//   2. Bấm vào một dòng -> dòng đó sáng lên, HAI NÚT DELETE/LOAD mới hiện ra
//      ở phía dưới. Chưa chọn dòng nào thì không có nút.
//   3. DELETE -> xoá file, danh sách tự nạp lại, bỏ chọn.
//      LOAD   -> nạp GameSaveData rồi chuyển cảnh vào màn chơi.
//
// HIỆU NĂNG: panel chỉ giữ vector<SaveSlotInfo> (đường dẫn + metadata nhẹ), KHÔNG
// giữ nội dung save. 20 bản lưu vẫn mở tức thì; nội dung thật chỉ được đọc khi
// bấm LOAD.
//
// SỞ HỮU TÀI NGUYÊN: tự Load/Unload texture của mình; Font là tài nguyên MƯỢN
// từ MapSelectionState nên không được Unload.
// =============================================================================
class SaveVersionPanel : public IMenuPanel {
private:
    bool isOpen_ = false;
    int  worldIndex_ = -1;
    std::string title_ = "";

    float screenW_ = 0.0f;
    float screenH_ = 0.0f;

    std::vector<SaveSlotInfo> slots_;
    int selectedIndex_ = -1;   // -1 = chưa chọn -> ẩn DELETE/LOAD

    Texture2D barTex_ = {0};
    Texture2D barPressTex_ = {0};

    Font font_ = {0};
    bool hasFont_ = false;

    Rectangle panelRect_ = {0, 0, 0, 0};
    Rectangle listRect_  = {0, 0, 0, 0};   // vùng cắt (scissor) của danh sách

    // Cuộn danh sách khi số bản lưu vượt quá chiều cao vùng hiển thị
    float scrollY_ = 0.0f;
    float maxScrollY_ = 0.0f;

    PanelButton deleteBtn_;
    PanelButton loadBtn_;
    PanelButton backBtn_;

    float animT_ = 0.0f;

    std::function<void(const SaveSlotInfo&)> onLoad_;
    std::function<void(const SaveSlotInfo&)> onDelete_;
    std::function<void()> onClose_;

    void RecalculateLayout();
    // Toạ độ dòng thứ i trong KHÔNG GIAN MÀN HÌNH (đã trừ scroll).
    Rectangle RowRect(int index) const;
    // Chuỗi hiển thị cho một dòng, ví dụ: "v3  Goku  World_Level_4  128c  240/300  12:05"
    std::string RowText(const SaveSlotInfo& slot) const;

public:
    SaveVersionPanel();
    ~SaveVersionPanel() override;

    void Init(float screenWidth, float screenHeight, Font font);

    // Mở panel với danh sách đã lấy sẵn từ SaveManager::listVersions().
    void Open(int worldIndex, const std::string& worldName, std::vector<SaveSlotInfo> slots);
    // Thay danh sách mà giữ panel mở (dùng sau khi xoá một bản lưu).
    void Refresh(std::vector<SaveSlotInfo> slots);
    void Close();
    bool IsOpen() const { return isOpen_; }
    int  GetWorldIndex() const { return worldIndex_; }

    void SetOnLoad(std::function<void(const SaveSlotInfo&)> callback) { onLoad_ = std::move(callback); }
    void SetOnDelete(std::function<void(const SaveSlotInfo&)> callback) { onDelete_ = std::move(callback); }
    void SetOnCloseCallback(std::function<void()> onClose) override { onClose_ = std::move(onClose); }

    void HandleInput(Vector2 mousePos, bool mousePressed, bool mouseReleased) override;
    void Update(float dt) override;
    void Render() const override;

private:
    // Trạng thái input được HandleInput ghi lại để Update xử lý — giữ đúng
    // phân tách 4 giai đoạn (HandleInput không được đổi selectedIndex_).
    Vector2 mousePos_ = {0, 0};
    bool pendingRowClick_ = false;
    int  pendingRowIndex_ = -1;
    float pendingWheel_ = 0.0f;
};
