#pragma once
#include "IEditorTool.h"

// Tool xóa tile VÀ entity tại ô được click/kéo.
//
// Thiết kế: EraseTool cũng là 1 Strategy để thống nhất interface,
// nhưng theo yêu cầu user: chuột PHẢI trong viewport luôn xóa bất kể
// tool nào đang active. EditorToolManager sẽ gọi EraseTool::erase()
// trực tiếp khi phát hiện IsMouseButtonDown(MOUSE_RIGHT_BUTTON).
class EraseTool : public IEditorTool {
public:
    void onPress  (int gx, int gy, CustomMapData& data) override;
    void onDrag   (int gx, int gy, CustomMapData& data) override;
    void onRelease() override {}

    // Static helper để EditorToolManager gọi trực tiếp khi chuột phải,
    // không cần set tool hiện tại sang EraseTool.
    static void erase(int gx, int gy, CustomMapData& data);

private:
    int lastGx_ = -1;
    int lastGy_ = -1;
};
