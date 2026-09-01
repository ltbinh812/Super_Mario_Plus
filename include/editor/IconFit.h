#pragma once
#include "raylib.h"
#include <cmath>
#include <algorithm>

// =============================================================================
// IconFit — Đặt một vùng ảnh nguồn vào một ô đích, GIỮ NGUYÊN tỉ lệ khung hình
// và căn giữa (kiểu "letterbox").
//
// Đoạn tính này trước đây bị chép nguyên văn ở 5 nơi: hai nhánh của
// MapEditorState::drawGhostPreview, TileMap::LoadCustomMap, BlockVariantPanel
// và EntityPalette. Chép nhiều bản nghĩa là sửa một chỗ thì 4 chỗ kia lệch đi.
//
// Dùng std::abs cho width/height vì UV có thể mang dấu âm để biểu thị lật ảnh.
// =============================================================================
class IconFit {
public:
    // srcUV  : vùng cắt trong ảnh nguồn (có thể âm nếu lật)
    // cell   : ô đích trên màn hình
    // Trả về hình chữ nhật đích đã căn giữa trong `cell`, đúng tỉ lệ của srcUV.
    static Rectangle fit(Rectangle srcUV, Rectangle cell) {
        const float sw = std::abs(srcUV.width);
        const float sh = std::abs(srcUV.height);
        if (sw <= 0.0f || sh <= 0.0f) return cell;

        const float scale = std::min(cell.width / sw, cell.height / sh);
        const float w = sw * scale;
        const float h = sh * scale;
        return { cell.x + (cell.width  - w) * 0.5f,
                 cell.y + (cell.height - h) * 0.5f,
                 w, h };
    }

    // Vẽ icon vừa khít ô. Nếu texture chưa nạp được (id == 0) thì vẽ ô màu dự
    // phòng để người dùng vẫn thấy có gì đó ở đấy, thay vì một khoảng trống.
    static void draw(const Texture2D& tex, Rectangle srcUV, Rectangle cell,
                     Color fallback, Color tint = WHITE) {
        if (tex.id != 0) {
            DrawTexturePro(tex, srcUV, fit(srcUV, cell), {0, 0}, 0.0f, tint);
        } else {
            DrawRectangleRec(cell, fallback);
        }
    }
};
