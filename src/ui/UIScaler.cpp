#include "UIScaler.h"
#include <algorithm>

UIScaler::UIScaler() {
    Refresh();
}

void UIScaler::Refresh() {
    float screenW = (float)GetScreenWidth();
    float screenH = (float)GetScreenHeight();

    // Phòng trường hợp được gọi trước khi cửa sổ kịp báo kích thước thật
    // (raylib chỉ cập nhật kích thước sau lần PollEvents đầu tiên, nên ngay
    // sau MaximizeWindow() giá trị có thể còn là 0).
    if (screenW <= 0.0f || screenH <= 0.0f) {
        screenW = kDesignWidth;
        screenH = kDesignHeight;
    }

    // min() giữ nguyên tỉ lệ khung hình -> ảnh không bị bóp méo, và nội dung
    // chắc chắn nằm lọt trong màn hình.
    factor_ = std::min(screenW / kDesignWidth, screenH / kDesignHeight);

    // Chia đôi phần dư để khung thiết kế nằm chính giữa màn hình.
    offsetX_ = (screenW - kDesignWidth  * factor_) * 0.5f;
    offsetY_ = (screenH - kDesignHeight * factor_) * 0.5f;
}
