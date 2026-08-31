#include "PlaceTileTool.h"

PlaceTileTool::PlaceTileTool(const std::string* pSelectedBlockId)
    : pSelected_(pSelectedBlockId) {}

#include "EditorBlockRegistry.h"

void PlaceTileTool::placeTile(int gx, int gy, CustomMapData& data) {
    if (!pSelected_ || pSelected_->empty()) return;
    
    auto& reg = EditorBlockRegistry::getInstance();
    if (!reg.has(*pSelected_)) return;
    const auto& def = reg.get(*pSelected_);
    
    int size = def.isStamp2x2 ? 2 : 1;
    
    for (int dy = 0; dy < size; ++dy) {
        for (int dx = 0; dx < size; ++dx) {
            int nx = gx + dx;
            int ny = gy + dy;
            if (!data.inBounds(nx, ny)) continue;
            
            if (data.getTile(nx, ny) != *pSelected_) {
                data.setTile(nx, ny, *pSelected_);
            }
        }
    }
}

void PlaceTileTool::onPress(int gx, int gy, CustomMapData& data) {
    lastGx_ = gx; lastGy_ = gy;
    placeTile(gx, gy, data);
}

void PlaceTileTool::onDrag(int gx, int gy, CustomMapData& data) {
    // Chỉ xử lý nếu ô thay đổi (tránh gọi thừa khi chuột đứng yên)
    if (gx == lastGx_ && gy == lastGy_) return;
    lastGx_ = gx; lastGy_ = gy;
    placeTile(gx, gy, data);
}
