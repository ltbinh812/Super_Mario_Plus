#include "EditorToolManager.h"
#include "PlaceTileTool.h"
#include "PlaceEntityTool.h"

EditorToolManager::EditorToolManager() {
    // Mặc định là PlaceTileTool, nhưng chưa có pSelected → pass nullptr
    // MapEditorState sẽ gọi setTool() sau khi khởi tạo các panel
}

void EditorToolManager::setTool(EditorToolType type,
                                const std::string* pSelectedBlockId,
                                const std::string* pSelectedEntityType) {
    activeType_ = type;
    switch (type) {
        case EditorToolType::Place:
            current_ = std::make_unique<PlaceTileTool>(pSelectedBlockId);
            break;
        case EditorToolType::PlaceEntity:
            current_ = std::make_unique<PlaceEntityTool>(pSelectedEntityType);
            break;
        case EditorToolType::Erase:
            current_ = std::make_unique<EraseTool>();
            break;
    }
    wasDragging_ = false;
}

bool EditorToolManager::dispatch(int gx, int gy,
                                  bool leftPress, bool leftDown, bool leftRelease,
                                  bool rightPress, bool rightDown,
                                  CustomMapData& data) {
    bool changed = false;

    // --- Chuột PHẢI: luôn xóa bất kể tool nào đang active ---
    if ((rightPress || rightDown) && gx >= 0 && gy >= 0) {
        EraseTool::erase(gx, gy, data);
        changed = true;
    }

    // --- Chuột TRÁI: dispatch vào current tool ---
    if (!current_) return changed;

    if (leftPress) {
        current_->onPress(gx, gy, data);
        wasDragging_ = true;
        changed = true;
    } else if (leftDown && wasDragging_) {
        current_->onDrag(gx, gy, data);
        changed = true;
    } else if (leftRelease) {
        current_->onRelease();
        wasDragging_ = false;
    }

    return changed;
}
