#pragma once
#include "CustomMapData.h"

// Interface Strategy cho các tool trong editor.
//
// Design: Strategy Pattern
// Mỗi tool (Place, Erase) implement interface này.
// EditorToolManager giữ 1 current tool và dispatch event vào nó.
//
// Lưu ý điều khiển chuột (xác nhận từ user):
//  - Chuột TRÁI  → Place (đặt block/entity) — luôn gọi qua tool hiện tại
//  - Chuột PHẢI  → Erase (xóa) — gọi trực tiếp, bất kể tool nào đang active
class IEditorTool {
public:
    virtual ~IEditorTool() = default;

    // Gọi khi người dùng nhấn chuột trái xuống tại ô (gx, gy)
    virtual void onPress(int gx, int gy, CustomMapData& data) = 0;

    // Gọi khi người dùng giữ chuột trái và kéo qua ô (gx, gy)
    virtual void onDrag(int gx, int gy, CustomMapData& data) = 0;

    // Gọi khi người dùng thả chuột trái
    virtual void onRelease() = 0;
};
