#pragma once
#include "IEditorTool.h"
#include "EraseTool.h"
#include <memory>

// Enum các loại tool khả dụng trong editor.
enum class EditorToolType { Place, PlaceEntity, Erase };

// Context của Strategy Pattern.
// Giữ tool hiện tại và dispatch mouse events.
//
// Quy tắc chuột:
//  - Chuột trái (PRESSED/DOWN) → dispatch tới current tool (Place/PlaceEntity)
//  - Chuột phải  (PRESSED/DOWN) → LUÔN xóa, bất kể tool nào đang active
class EditorToolManager {
public:
    EditorToolManager();

    // Đổi tool. Tạo lại concrete Strategy tương ứng.
    void setTool(EditorToolType type,
                 const std::string* pSelectedBlockId,
                 const std::string* pSelectedEntityType);

    EditorToolType getActiveTool() const { return activeType_; }

    // Gọi mỗi frame trong Process():
    //  - gx, gy: vị trí grid hiện tại của chuột
    //  - leftPress/leftDown/leftRelease: trạng thái chuột trái
    //  - rightPress/rightDown: trạng thái chuột phải (→ luôn erase)
    //  - data: map data cần modify
    // Trả về true nếu có thay đổi (để MapEditorState biết set dirty flag).
    bool dispatch(int gx, int gy,
                  bool leftPress, bool leftDown, bool leftRelease,
                  bool rightPress, bool rightDown,
                  CustomMapData& data);

private:
    EditorToolType            activeType_ = EditorToolType::Place;
    std::unique_ptr<IEditorTool> current_;
    EraseTool                 eraseTool_;  // singleton-like instance cho right click
    bool                      wasDragging_ = false;
};
