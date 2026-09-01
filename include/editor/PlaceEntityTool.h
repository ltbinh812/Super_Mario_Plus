#pragma once
#include "IEditorTool.h"
#include <functional>
#include <string>

struct EntityDef;

// Tool đặt entity vào map (Coin, Flag, PlayerSpawn, Mob_*, Boss_*, ...).
// Mỗi ô grid chỉ chứa 1 entity, không đặt đè lên entity đã có.
class PlaceEntityTool : public IEditorTool {
public:
    // pSelectedEntityType : con trỏ tới entity type đang chọn trong EntityPalette.
    // defLookup           : tra EntityDef theo id, dùng để vẽ ghost.
    //                       Truyền dưới dạng std::function để tool KHÔNG phải
    //                       phụ thuộc ngược lên EntityPalette (tránh vòng phụ thuộc).
    PlaceEntityTool(const std::string* pSelectedEntityType,
                    std::function<const EntityDef*(const std::string&)> defLookup = nullptr);

    bool onPress(int gx, int gy, CustomMapData& data) override;
    // Entity chỉ đặt khi click, không đặt theo vệt kéo.
    bool onDrag (int /*gx*/, int /*gy*/, CustomMapData& /*data*/) override { return false; }
    void onRelease() override {}

    void renderGhost(int gx, int gy, Rectangle cellRect,
                     const CustomMapData& data) const override;

private:
    const std::string* pSelected_;
    std::function<const EntityDef*(const std::string&)> defLookup_;
};
