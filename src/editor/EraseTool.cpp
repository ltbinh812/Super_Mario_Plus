#include "EraseTool.h"

bool EraseTool::eraseAt(int gx, int gy, CustomMapData& data) {
    if (!data.inBounds(gx, gy)) return false;

    // Ô trống thì không có gì để xoá — và quan trọng hơn, KHÔNG được báo là
    // "đã thay đổi", nếu không mỗi cú click vào chỗ trống lại nhét một mốc Undo
    // rỗng và bắt dựng lại toàn bộ canvas.
    const bool hadTile   = !data.getTile(gx, gy).empty();
    const bool hadEntity = data.hasEntityAt(gx, gy);
    if (!hadTile && !hadEntity) return false;

    if (hadTile)   data.setTile(gx, gy, "");
    if (hadEntity) data.eraseEntityAt(gx, gy);
    return true;
}

bool EraseTool::onPress(int gx, int gy, CustomMapData& data) {
    lastGx_ = gx; lastGy_ = gy;
    return eraseAt(gx, gy, data);
}

bool EraseTool::onDrag(int gx, int gy, CustomMapData& data) {
    if (gx == lastGx_ && gy == lastGy_) return false;
    lastGx_ = gx; lastGy_ = gy;
    return eraseAt(gx, gy, data);
}

void EraseTool::renderGhost(int gx, int gy, Rectangle cellRect,
                            const CustomMapData& data) const {
    // Ô trống thì làm mờ ghost đi, cho thấy ở đây không có gì để xoá.
    const bool hasSomething = !data.getTile(gx, gy).empty() || data.hasEntityAt(gx, gy);
    const unsigned char a = hasSomething ? 50 : 20;
    const unsigned char b = hasSomething ? 180 : 70;

    DrawRectangleRec(cellRect, Color{255, 60, 60, a});
    DrawRectangleLinesEx(cellRect, 2.0f, Color{255, 80, 80, b});
}
