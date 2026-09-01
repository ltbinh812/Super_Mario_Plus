#include "PlaceEntityTool.h"
#include "EntityDef.h"
#include "EditorTextureCache.h"
#include "IconFit.h"

PlaceEntityTool::PlaceEntityTool(const std::string* pSelectedEntityType,
                                 std::function<const EntityDef*(const std::string&)> defLookup)
    : pSelected_(pSelectedEntityType), defLookup_(std::move(defLookup)) {}

bool PlaceEntityTool::onPress(int gx, int gy, CustomMapData& data) {
    if (!data.inBounds(gx, gy)) return false;
    if (!pSelected_ || pSelected_->empty()) return false;

    // Không đặt đè: ô đã có entity thì bỏ qua, và KHÔNG coi là thay đổi
    // (nếu báo là đã đổi thì mỗi cú click hụt lại nhét một mốc Undo rỗng).
    // Cũng không in log ở đây: đây là đường đi của input, giữ chuột trên một ô
    // đã có entity sẽ làm ngập console.
    if (data.hasEntityAt(gx, gy)) return false;

    CustomEntityData e;
    e.type   = *pSelected_;
    e.gridX  = gx;
    e.gridY  = gy;
    // MẢNG, không phải object — ItemFactory/EnemyFactory chỉ đọc fieldInstances
    // khi is_array(). Lưu dạng object là mọi field bị bỏ qua âm thầm.
    e.fields = nlohmann::json::array();
    data.entities.push_back(std::move(e));
    return true;
}

void PlaceEntityTool::renderGhost(int gx, int gy, Rectangle cellRect,
                                  const CustomMapData& data) const {
    if (!pSelected_ || pSelected_->empty()) return;

    const Color ghostTint = {255, 255, 255, 130};

    // Ô đã có entity -> báo bằng viền đỏ cho biết cú click sẽ không ăn,
    // thay vì để người dùng bấm mãi mà không hiểu vì sao không có gì xảy ra.
    if (data.hasEntityAt(gx, gy)) {
        DrawRectangleRec(cellRect, Color{255, 60, 60, 60});
        DrawRectangleLinesEx(cellRect, 2.0f, Color{255, 80, 80, 180});
        return;
    }

    const EntityDef* def = defLookup_ ? defLookup_(*pSelected_) : nullptr;
    if (!def) {
        DrawRectangleLinesEx(cellRect, 2.0f, ghostTint);
        return;
    }

    const Color fallbackGhost = {def->fallbackColor.r, def->fallbackColor.g,
                                 def->fallbackColor.b, 100};
    if (!def->texturePath.empty()) {
        const Texture2D& tex = EditorTextureCache::getInstance().getOrDefault(def->texturePath);
        IconFit::draw(tex, def->uv, cellRect, fallbackGhost, ghostTint);
    } else {
        DrawRectangleRec(cellRect, fallbackGhost);
    }
    DrawRectangleLinesEx(cellRect, 2.0f, ghostTint);
}
