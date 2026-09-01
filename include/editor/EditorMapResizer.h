#pragma once
#include "CustomMapData.h"
#include "raylib.h"

// Xử lý các nút kéo trên 4 cạnh biên map để resize.
//
// Mỗi cạnh (N/S/E/W) có 1 handle hình tam giác.
// Kéo ra (xa trung tâm map) → thêm row/col EMPTY
// Kéo vào (gần trung tâm) → xóa row/col ngoài cùng
//
// Tọa độ handle tính trong world space (sau camera transform).
class EditorMapResizer {
public:
    static constexpr int   HANDLE_SIZE = 16;   // px trong world space
    static constexpr int   MIN_SIZE    = 10;   // tile tối thiểu mỗi chiều
    // Trần kích thước. Trước đây CHỈ có MIN_SIZE, nên một cú kéo ở mức zoom
    // thấp (một pixel màn hình = nhiều ô) có thể phóng map lên hàng nghìn ô ->
    // LoadRenderTexture khổng lồ + AutoTiler quét vài giây mỗi frame -> treo.
    static constexpr int   MAX_SIZE    = 500;

    // Vẽ 4 handle trên 4 cạnh biên. Gọi trong BeginMode2D.
    // worldTileSize = kích thước 1 tile sau scale (thường 32px)
    void render(const CustomMapData& data, float worldTileSize) const;

    // Xử lý drag input. Gọi trong Process(). Modifies data nếu resize.
    // mouseWorld: tọa độ chuột trong world space (từ EditorCamera::screenToWorld).
    // Trả về true nếu có thay đổi kích thước map.
    bool process(CustomMapData& data, float worldTileSize, Vector2 mouseWorld);

    // Con trỏ có đang nằm trên một handle không? Bên gọi dùng để ghi mốc Undo
    // TRƯỚC khi phép resize bắt đầu làm thay đổi dữ liệu.
    bool isHoveringHandle(const CustomMapData& data, float worldTileSize, Vector2 mouseWorld) const;

    // Đang trong một thao tác kéo resize?
    bool isDragging() const { return dragging_ != Handle::None; }

    // Reset trạng thái drag (gọi khi rời editor)
    void reset();

private:
    enum class Handle { None, North, South, East, West };

    Handle  dragging_    = Handle::None;
    Vector2 dragStart_   = {0, 0};
    int     dragStartDim_= 0;   // width hoặc height ban đầu khi bắt đầu drag

    // Tính Rectangle của mỗi handle trong world space
    Rectangle getHandleRect(Handle h, const CustomMapData& data, float worldTileSize) const;

    // Thêm/xóa row hoặc col, fill EMPTY nếu mở rộng
    static void expandNorth(CustomMapData& data);
    static void expandSouth(CustomMapData& data);
    static void expandEast (CustomMapData& data);
    static void expandWest (CustomMapData& data);
    static void shrinkNorth(CustomMapData& data);
    static void shrinkSouth(CustomMapData& data);
    static void shrinkEast (CustomMapData& data);
    static void shrinkWest (CustomMapData& data);
};
