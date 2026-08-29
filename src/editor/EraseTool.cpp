#include "EraseTool.h"

void EraseTool::erase(int gx, int gy, CustomMapData& data) {
    if (!data.inBounds(gx, gy)) return;
    data.setTile(gx, gy, "");   // xóa tile (setTile với "" = erase từ sparse map)
    data.eraseEntityAt(gx, gy); // xóa entity tại ô đó nếu có
}

void EraseTool::onPress(int gx, int gy, CustomMapData& data) {
    lastGx_ = gx; lastGy_ = gy;
    erase(gx, gy, data);
}

void EraseTool::onDrag(int gx, int gy, CustomMapData& data) {
    if (gx == lastGx_ && gy == lastGy_) return;
    lastGx_ = gx; lastGy_ = gy;
    erase(gx, gy, data);
}
