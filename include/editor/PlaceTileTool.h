#pragma once
#include "IEditorTool.h"
#include <string>

// Tool đặt tile vào map.
// Hoạt động với cả click đơn và kéo (giữ + di chuyển chuột).
// Không đặt lại nếu ô đó đã đúng blockId đang chọn — nhờ vậy trả về false và
// bên gọi bỏ qua cả mốc Undo lẫn việc dựng lại canvas.
class PlaceTileTool : public IEditorTool {
public:
    // pSelectedBlockId: con trỏ tới selected block ID trong EditorBottomPanel.
    // Dùng pointer để PlaceTileTool luôn đọc block đang chọn mới nhất.
    explicit PlaceTileTool(const std::string* pSelectedBlockId);

    bool onPress(int gx, int gy, CustomMapData& data) override;
    bool onDrag (int gx, int gy, CustomMapData& data) override;
    void onRelease() override { lastGx_ = -1; lastGy_ = -1; }

    void renderGhost(int gx, int gy, Rectangle cellRect,
                     const CustomMapData& data) const override;

private:
    // Trả true nếu có ít nhất một ô thực sự đổi giá trị.
    bool placeTile(int gx, int gy, CustomMapData& data);

    const std::string* pSelected_;
    int lastGx_ = -1;
    int lastGy_ = -1;
};
