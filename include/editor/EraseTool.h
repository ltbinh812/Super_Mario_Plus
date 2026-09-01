#pragma once
#include "IEditorTool.h"

// =============================================================================
// EraseTool — Xoá tile VÀ entity tại ô được click/kéo.
//
// Chuột PHẢI trong viewport luôn xoá, bất kể tool nào đang active. Trước đây
// EditorToolManager thực hiện việc đó bằng cách gọi thẳng hàm TĨNH
// EraseTool::erase(), tức là đi vòng qua Strategy: instance `eraseTool_` khai
// báo sẵn trong manager không bao giờ được dùng, và vì không đi qua instance
// nên `lastGx_/lastGy_` cũng vô tác dụng — giữ chuột phải đứng yên một ô vẫn
// xoá lại ô đó mỗi frame và báo "map đã đổi", kéo theo dựng lại toàn bộ canvas
// 60 lần/giây.
//
// Nay manager giữ một EraseTool THẬT cho chuột phải và gọi qua interface.
// =============================================================================
class EraseTool : public IEditorTool {
public:
    bool onPress  (int gx, int gy, CustomMapData& data) override;
    bool onDrag   (int gx, int gy, CustomMapData& data) override;
    void onRelease() override { lastGx_ = -1; lastGy_ = -1; }

    void renderGhost(int gx, int gy, Rectangle cellRect,
                     const CustomMapData& data) const override;

private:
    // Trả true nếu ô thực sự có gì đó để xoá.
    static bool eraseAt(int gx, int gy, CustomMapData& data);

    int lastGx_ = -1;
    int lastGy_ = -1;
};
