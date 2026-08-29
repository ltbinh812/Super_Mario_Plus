#pragma once
#include "raylib.h"

// =============================================================================
// EditorCamera — Wrapper quanh Camera2D của raylib cho Map Editor.
//
// Chiến lược camera:
//   cam_.target  = điểm world mà camera đang nhìn vào (trung tâm)
//   cam_.offset  = luôn = {screenW/2, screenH/2} — cố định ở center
//
// Tính năng:
//   - Pan: giữ chuột giữa + kéo → thay đổi cam_.target
//   - Zoom: scroll wheel → thay đổi cam_.zoom (0.5x → 4.0x), zoom về điểm con trỏ
//   - Clamp: camera không di chuyển ra ngoài map + MARGIN_PX
// =============================================================================
class EditorCamera {
public:
    EditorCamera();

    // Pan camera để nhìn vào điểm world (gọi trong ctor để center map)
    void panTo(float worldX, float worldY);

    // Cập nhật camera từ input (gọi trong Update)
    // mapPixelW, mapPixelH: kích thước map tính bằng pixel (tiles * tileSize * 2)
    void update(float dt, int mapPixelW, int mapPixelH, bool blockInput = false);

    // Bắt đầu/kết thúc camera transform (gọi trong Render)
    void beginMode() const;
    void endMode()   const;

    // Chuyển tọa độ màn hình → world
    Vector2 screenToWorld(Vector2 screenPos) const;

    // Reset về (0,0) zoom=1
    void reset();

    const Camera2D& get()     const { return cam_; }
    float           getZoom() const { return cam_.zoom; }

private:
    Camera2D cam_;

    // Pan state
    bool    isPanning_      = false;
    Vector2 panStart_       = {0, 0};
    Vector2 camStartTarget_ = {0, 0};  // cam_.target khi bắt đầu pan

    static constexpr float ZOOM_MIN  = 0.25f;
    static constexpr float ZOOM_MAX  = 4.0f;
    static constexpr float ZOOM_STEP = 0.1f;
    static constexpr float MARGIN_PX = 200.0f;  // vùng đệm ngoài biên map
};
