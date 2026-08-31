#include "PlaceEntityTool.h"
#include <iostream>

PlaceEntityTool::PlaceEntityTool(const std::string* pSelectedEntityType)
    : pSelected_(pSelectedEntityType) {}

void PlaceEntityTool::onPress(int gx, int gy, CustomMapData& data) {
    if (!data.inBounds(gx, gy)) return;
    if (!pSelected_ || pSelected_->empty()) return;

    // Không đặt đè: nếu đã có entity tại ô này, bỏ qua
    if (data.hasEntityAt(gx, gy)) {
        std::cout << "[PlaceEntityTool] Cell (" << gx << "," << gy << ") already has entity.\n";
        return;
    }

    CustomEntityData e;
    e.type  = *pSelected_;
    e.gridX = gx;
    e.gridY = gy;
    e.fields = nlohmann::json::object();
    data.entities.push_back(e);
}
