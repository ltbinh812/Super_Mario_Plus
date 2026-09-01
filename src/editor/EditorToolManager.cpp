#include "EditorToolManager.h"
#include "PlaceTileTool.h"
#include "PlaceEntityTool.h"

void EditorToolManager::setTool(EditorToolType type,
                                const std::string* pSelectedBlockId,
                                const std::string* pSelectedEntityType,
                                std::function<const EntityDef*(const std::string&)> defLookup) {
    activeType_ = type;
    switch (type) {
        case EditorToolType::Place:
            current_ = std::make_unique<PlaceTileTool>(pSelectedBlockId);
            break;
        case EditorToolType::PlaceEntity:
            current_ = std::make_unique<PlaceEntityTool>(pSelectedEntityType, std::move(defLookup));
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

    // --- Chuột PHẢI: luôn xoá, đi qua một EraseTool thật để tận dụng dedupe ô ---
    if (rightPress) {
        changed |= rightEraseTool_.onPress(gx, gy, data);
        wasRightErasing_ = true;
    } else if (rightDown && wasRightErasing_) {
        changed |= rightEraseTool_.onDrag(gx, gy, data);
    } else if (wasRightErasing_ && !rightDown) {
        rightEraseTool_.onRelease();
        wasRightErasing_ = false;
    }

    // --- Chuột TRÁI: tool đang chọn ---
    if (!current_) return changed;

    if (leftPress) {
        changed |= current_->onPress(gx, gy, data);
        wasDragging_ = true;
    } else if (leftDown && wasDragging_) {
        changed |= current_->onDrag(gx, gy, data);
    } else if (leftRelease || (!leftDown && wasDragging_)) {
        // Nhả chuột NGOÀI vùng map cũng phải kết thúc thao tác kéo. Nếu chỉ dựa
        // vào leftRelease, kéo ra khỏi map rồi nhả sẽ để wasDragging_ kẹt ở true.
        current_->onRelease();
        wasDragging_ = false;
    }

    return changed;
}
