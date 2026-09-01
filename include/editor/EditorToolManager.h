#pragma once
#include "IEditorTool.h"
#include "EraseTool.h"
#include "EditorToolType.h"
#include "EntityDef.h"
#include <functional>
#include <memory>

// =============================================================================
// EditorToolManager — Context của Strategy Pattern.
// Giữ tool hiện hành và điều phối sự kiện chuột vào nó.
//
// Quy tắc chuột:
//  - Chuột TRÁI  → tool đang chọn (Place / PlaceEntity / Erase)
//  - Chuột PHẢI  → LUÔN xoá, bất kể tool nào đang chọn — thực hiện qua
//                  rightEraseTool_ (một EraseTool thật), KHÔNG gọi hàm tĩnh.
// =============================================================================
class EditorToolManager {
public:
    EditorToolManager() = default;

    // Đổi tool. defLookup dùng cho ghost của PlaceEntityTool.
    void setTool(EditorToolType type,
                 const std::string* pSelectedBlockId,
                 const std::string* pSelectedEntityType,
                 std::function<const EntityDef*(const std::string&)> defLookup = nullptr);

    EditorToolType getActiveTool() const { return activeType_; }

    // Gọi mỗi frame trong Process().
    // Trả true CHỈ KHI dữ liệu map thực sự thay đổi — bên gọi dựa vào đó để
    // quyết định ghi mốc Undo và dựng lại canvas.
    bool dispatch(int gx, int gy,
                  bool leftPress, bool leftDown, bool leftRelease,
                  bool rightPress, bool rightDown,
                  CustomMapData& data);

    // Vẽ ghost của tool đang chọn. Uỷ quyền hoàn toàn cho tool.
    void renderGhost(int gx, int gy, Rectangle cellRect, const CustomMapData& data) const {
        if (current_) current_->renderGhost(gx, gy, cellRect, data);
    }

private:
    EditorToolType               activeType_ = EditorToolType::Place;
    std::unique_ptr<IEditorTool> current_;
    EraseTool                    rightEraseTool_;  // dành riêng cho chuột phải
    bool                         wasDragging_ = false;
    bool                         wasRightErasing_ = false;
};
