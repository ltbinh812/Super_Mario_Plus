#pragma once
#include "IEditorTool.h"
#include <string>

// Tool đặt entity vào map (Coin, Flag, PlayerSpawn, v.v.)
// Mỗi ô grid chỉ chứa 1 entity. Không đặt đè lên entity đã có.
class PlaceEntityTool : public IEditorTool {
public:
    // pSelectedEntityType: con trỏ tới entity type đang chọn trong EntityPalettePanel.
    explicit PlaceEntityTool(const std::string* pSelectedEntityType);

    void onPress(int gx, int gy, CustomMapData& data) override;
    void onDrag (int gx, int gy, CustomMapData& data) override {}  // Entity: chỉ đặt khi click, không kéo
    void onRelease() override {}

private:
    const std::string* pSelected_;
};
