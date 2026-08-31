#pragma once
#include "raylib.h"

// =============================================================================
// UIScaler — Quy đổi toạ độ/kích thước từ "khung thiết kế ảo" sang màn hình thật.
//
// VẤN ĐỀ NÓ GIẢI QUYẾT
// Trước đây MainMenuState viết thẳng các hằng số pixel: panelScale = 3.3f,
// paddingLeft = 220.0f, toggle ở {30, 90}... Những con số này được canh mắt
// trên một màn hình cụ thể. Sang máy có độ phân giải khác, ảnh vẫn giữ nguyên
// số pixel cũ trong khi màn hình to/nhỏ đi -> bố cục lệch hết.
//
// GIẢI PHÁP: KHUNG THIẾT KẾ ẢO (virtual canvas)
// Coi như mọi thứ được vẽ trong một khung cố định, rồi phóng toàn bộ khung đó
// lên cho vừa màn hình thật:
//
//     factor = min(screenW / kDesignWidth, screenH / kDesignHeight)
//
// Dùng min (chứ không phải scaleX/scaleY riêng) vì hai lý do:
//   1. Giữ nguyên tỉ lệ khung hình -> ảnh KHÔNG bị bóp méo. Nút vuông vẫn vuông.
//   2. Bảo đảm nội dung không bao giờ tràn ra khỏi màn hình.
//
// Phần dư ra ở hai bên (khi tỉ lệ màn hình khác 16:9) được chia đôi thành
// offsetX/offsetY, nên khung thiết kế luôn nằm chính giữa màn hình.
//
//     screenW
//   |<--------------------------------->|
//   |  offsetX |   1280*factor  | offsetX|
//   +----------+----------------+--------+
//
// CÁCH DÙNG
//     UIScaler ui;                       // tự đọc kích thước màn hình hiện tại
//     float scale   = ui.S(3.3f);        // đổi một ĐỘ DÀI / hệ số phóng
//     Vector2 pos   = ui.Pos(220, 100);  // đổi một TOẠ ĐỘ (có cộng offset)
//     Rectangle box = ui.Rect(30, 90, 60, 20);
//
// PHÂN BIỆT S() VÀ X()/Y(): S() chỉ nhân hệ số (dùng cho chiều dài, bán kính,
// cỡ chữ, độ dày nét). X()/Y() nhân hệ số RỒI cộng offset (dùng cho toạ độ).
// Nhầm hai cái này là nguồn lỗi bố cục phổ biến nhất.
//
// LƯU Ý: object này chụp kích thước màn hình tại thời điểm gọi Refresh().
// Cửa sổ game có cờ FLAG_WINDOW_RESIZABLE, nên nếu muốn bố cục chạy theo lúc
// người dùng kéo giãn cửa sổ thì phải gọi lại Refresh() và dựng lại layout.
// =============================================================================
class UIScaler {
private:
    float factor_  = 1.0f;
    float offsetX_ = 0.0f;
    float offsetY_ = 0.0f;

public:
    // ĐỘ PHÂN GIẢI THIẾT KẾ — mọi hằng số pixel trong UI được canh theo khung này.
    //
    // ⚠ ĐỪNG NHẦM VỚI InitWindow(1280, 720) TRONG main.cpp.
    // main.cpp mở cửa sổ 1280x720 rồi gọi ngay MaximizeWindow(), nên kích thước
    // thật lúc chạy là vùng làm việc của màn hình, KHÔNG phải 1280x720. Trên máy
    // đã canh giao diện này (màn 2560x1600, Windows scale 200%) raylib báo
    // 2560x1459 pixel vật lý — lưu ý raylib đếm pixel vật lý, còn Windows báo
    // 1280x800 pixel logic, hai con số này khác nhau khi bật DPI scaling.
    //
    // Chọn 2560x1440 (QHD 16:9 chuẩn) làm khung thiết kế vì:
    //   - Ở 2560x1459 -> factor = min(1.000, 1.013) = 1.000, tức giao diện giữ
    //     NGUYÊN XI kích thước hiện tại, không xê dịch một pixel nào.
    //   - Là con số tròn, dễ đọc, không phụ thuộc chiều cao taskbar của một máy.
    //
    // MUỐN CANH LẠI GIAO DIỆN: sửa các hằng số trong MainMenuState ở đúng khung
    // 2560x1440 này, hoặc đổi hai giá trị dưới đây cho khớp màn hình bạn dùng
    // để canh. Đây là hai con số duy nhất cần chỉnh.
    static constexpr float kDesignWidth  = 2560.0f;
    static constexpr float kDesignHeight = 1440.0f;

    UIScaler();

    // Đọc lại kích thước màn hình và tính lại factor/offset.
    void Refresh();

    float Factor()  const { return factor_; }
    float OffsetX() const { return offsetX_; }
    float OffsetY() const { return offsetY_; }

    // Chiều rộng/cao của khung thiết kế sau khi phóng (không tính phần lề).
    float ViewWidth()  const { return kDesignWidth  * factor_; }
    float ViewHeight() const { return kDesignHeight * factor_; }

    // Tâm khung thiết kế trên màn hình thật.
    float CenterX() const { return offsetX_ + ViewWidth()  * 0.5f; }
    float CenterY() const { return offsetY_ + ViewHeight() * 0.5f; }

    // Đổi một ĐỘ DÀI (không cộng offset).
    float S(float designLength) const { return designLength * factor_; }

    // Đổi một TOẠ ĐỘ (có cộng offset).
    float X(float designX) const { return offsetX_ + designX * factor_; }
    float Y(float designY) const { return offsetY_ + designY * factor_; }

    Vector2 Pos(float designX, float designY) const {
        return { X(designX), Y(designY) };
    }

    Rectangle Rect(float designX, float designY, float designW, float designH) const {
        return { X(designX), Y(designY), S(designW), S(designH) };
    }
};
