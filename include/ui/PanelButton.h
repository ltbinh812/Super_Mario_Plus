#pragma once
#include "raylib.h"
#include <string>

// =============================================================================
// PanelButton — Nút bấm dùng chung cho các panel overlay (WorldActionPanel,
// SaveVersionPanel).
//
// VÌ SAO KHÔNG DÙNG LẠI class Button (include/ui/Buttons.h)?
// Button hiện có vẽ bằng DrawRectangleRounded + DrawText với font mặc định của
// raylib, và tự gọi onClick_() bên trong update(). Hai panel mới cần:
//   - vẽ bằng texture pixel-art (bar.png / bar_press.png) cho khớp menu game
//   - dùng font riêng kenney-pixel-hu.otf
//   - hiệu ứng phóng to khi rê chuột
//   - và quan trọng nhất: TRẢ VỀ sự kiện click thay vì tự xử lý
//
// Điểm cuối cùng là để giữ đúng quy tắc 4 giai đoạn của dự án:
//     HandleInput()  chỉ GHI NHẬN là đã bấm  -> đặt cờ clicked_
//     ConsumeClick() được panel gọi ở giai đoạn xử lý logic -> lấy cờ ra và xoá
// Nhờ vậy không có logic game nào chạy lén trong giai đoạn đọc input.
//
// SỞ HỮU TÀI NGUYÊN: PanelButton KHÔNG sở hữu Texture2D và Font được truyền
// vào — nó chỉ mượn để vẽ. Bên gọi (panel) chịu trách nhiệm Load/Unload.
// =============================================================================
class PanelButton {
private:
    Rectangle bounds_ = {0, 0, 0, 0};
    std::string label_ = "";

    Texture2D texNormal_ = {0};
    Texture2D texPressed_ = {0};
    Font font_ = {0};
    bool hasFont_ = false;

    Color labelColor_ = BLACK;
    float fontSize_ = 24.0f;

    bool enabled_ = true;
    bool hovered_ = false;
    bool pressed_ = false;
    bool clicked_ = false;   // cờ chờ được ConsumeClick() lấy đi

    float hoverScale_ = 1.0f; // nội suy mượt về 1.0 hoặc 1.06

public:
    PanelButton() = default;

    // --- Cấu hình (gọi lúc dựng layout) ---
    void SetBounds(Rectangle bounds) { bounds_ = bounds; }
    void SetLabel(const std::string& label) { label_ = label; }
    void SetTextures(Texture2D normal, Texture2D pressed) { texNormal_ = normal; texPressed_ = pressed; }
    void SetFont(Font font) { font_ = font; hasFont_ = true; }
    void SetFontSize(float size) { fontSize_ = size; }
    void SetLabelColor(Color color) { labelColor_ = color; }
    void SetEnabled(bool enabled);

    // --- Truy vấn ---
    Rectangle GetBounds() const { return bounds_; }
    bool IsHovered() const { return hovered_; }
    bool IsEnabled() const { return enabled_; }

    // --- 4 giai đoạn ---
    void HandleInput(Vector2 mousePos, bool mousePressed, bool mouseReleased);
    void Update(float dt);
    void Render() const;

    // Trả true ĐÚNG MỘT LẦN cho mỗi lần bấm; gọi ở giai đoạn xử lý logic.
    bool ConsumeClick();

    // Xoá sạch trạng thái hover/press — dùng khi panel đóng lại giữa chừng để
    // lần mở sau nút không bị "kẹt" ở trạng thái đang bấm.
    void Reset();
};
