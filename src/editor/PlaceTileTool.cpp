#include "PlaceTileTool.h"
#include "EditorBlockRegistry.h"
#include "EditorTextureCache.h"
#include "IconFit.h"

PlaceTileTool::PlaceTileTool(const std::string* pSelectedBlockId)
    : pSelected_(pSelectedBlockId) {}

bool PlaceTileTool::placeTile(int gx, int gy, CustomMapData& data) {
    if (!pSelected_ || pSelected_->empty()) return false;

    auto& reg = EditorBlockRegistry::getInstance();
    if (!reg.has(*pSelected_)) return false;
    const auto& def = reg.get(*pSelected_);

    const int size = def.isStamp2x2 ? 2 : 1;

    bool changed = false;
    for (int dy = 0; dy < size; ++dy) {
        for (int dx = 0; dx < size; ++dx) {
            const int nx = gx + dx;
            const int ny = gy + dy;
            if (!data.inBounds(nx, ny)) continue;
            // Đặt lại đúng thứ đã có ở đó KHÔNG phải là một thay đổi.
            if (data.getTile(nx, ny) == *pSelected_) continue;
            data.setTile(nx, ny, *pSelected_);
            changed = true;
        }
    }
    return changed;
}

bool PlaceTileTool::onPress(int gx, int gy, CustomMapData& data) {
    lastGx_ = gx; lastGy_ = gy;
    return placeTile(gx, gy, data);
}

bool PlaceTileTool::onDrag(int gx, int gy, CustomMapData& data) {
    // Chuột đứng yên trong cùng một ô -> không có gì để làm.
    if (gx == lastGx_ && gy == lastGy_) return false;
    lastGx_ = gx; lastGy_ = gy;
    return placeTile(gx, gy, data);
}

void PlaceTileTool::renderGhost(int /*gx*/, int /*gy*/, Rectangle cellRect,
                                const CustomMapData& /*data*/) const {
    if (!pSelected_ || pSelected_->empty()) return;

    auto& reg = EditorBlockRegistry::getInstance();
    if (!reg.has(*pSelected_)) return;
    const auto& def = reg.get(*pSelected_);

    const Color ghostTint     = {255, 255, 255, 130};
    const Color fallbackGhost = {def.fallbackColor.r, def.fallbackColor.g,
                                 def.fallbackColor.b, 100};

    if (!def.tilesetPath.empty()) {
        const Texture2D& tex = EditorTextureCache::getInstance().getOrDefault(def.tilesetPath);
        IconFit::draw(tex, def.uv, cellRect, fallbackGhost, ghostTint);
    } else {
        DrawRectangleRec(cellRect, fallbackGhost);
    }
    DrawRectangleLinesEx(cellRect, 2.0f, ghostTint);
}
