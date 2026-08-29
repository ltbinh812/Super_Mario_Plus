#pragma once
#include "IEditorTool.h"
#include <string>

// Tool đặt tile vào map.
// Hoạt động với cả click đơn và kéo (giữ + di chuyển chuột).
// Không đặt nếu ô đó đã có cùng blockId (tránh dirty flag thừa).
class PlaceTileTool : public IEditorTool {
public:
    // pSelectedBlockId: con trỏ tới selected block ID trong EditorBottomPanel.
    // Dùng pointer để PlaceTileTool luôn đọc block đang chọn mới nhất.
    explicit PlaceTileTool(const std::string* pSelectedBlockId);

    void onPress(int gx, int gy, CustomMapData& data) override;
    void onDrag (int gx, int gy, CustomMapData& data) override;
    void onRelease() override {}

private:
    void placeTile(int gx, int gy, CustomMapData& data);
    const std::string* pSelected_;
    int lastGx_ = -1;
    int lastGy_ = -1;
};
