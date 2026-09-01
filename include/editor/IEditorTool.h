#pragma once
#include "CustomMapData.h"
#include "raylib.h"   // Rectangle, Color — dùng trong renderGhost()

// =============================================================================
// IEditorTool — Strategy cho các công cụ trong editor (Place, PlaceEntity, Erase).
//
// EditorToolManager giữ một tool hiện hành và dispatch sự kiện chuột vào nó.
//
// Điều khiển chuột:
//  - Chuột TRÁI  → tool đang chọn
//  - Chuột PHẢI  → luôn là Erase, bất kể tool nào đang active
//
// VÌ SAO onPress/onDrag TRẢ VỀ bool?
// Bên gọi cần biết thao tác có THỰC SỰ đổi dữ liệu hay không:
//   1. Chỉ ghi một mốc Undo khi có thay đổi. Trước đây mốc được ghi ngay lúc
//      nhấn chuột, nên click vào ô đã có sẵn block / ô đã có entity / click khi
//      chưa chọn block đều nhét một bản chụp rỗng vào lịch sử. Trần lịch sử là
//      50 bước, nên vài cú click vu vơ là đẩy văng hết thao tác thật, và Ctrl+Z
//      trông như bị liệt.
//   2. Chỉ dựng lại canvas + chạy lại AutoTiler khi có thay đổi. Trước đây
//      dispatch() luôn báo "đã đổi", nên chỉ cần GIỮ chuột đứng yên một chỗ là
//      mỗi frame lại huỷ/tạo lại render texture toàn map và quét lại ~400 rule.
// =============================================================================
class IEditorTool {
public:
    virtual ~IEditorTool() = default;

    // Nhấn chuột trái tại ô (gx, gy). Trả true nếu dữ liệu map đã thay đổi.
    virtual bool onPress(int gx, int gy, CustomMapData& data) = 0;

    // Giữ và kéo chuột trái qua ô (gx, gy). Trả true nếu dữ liệu đã thay đổi.
    virtual bool onDrag(int gx, int gy, CustomMapData& data) = 0;

    // Thả chuột trái — dọn trạng thái kéo nội bộ của tool.
    virtual void onRelease() = 0;

    // Vẽ xem trước (ghost) của tool tại ô đang trỏ.
    //
    // Trước đây phần này là một chuỗi if/else 80 dòng theo enum tool nằm trong
    // MapEditorState — mỗi tool mới lại buộc phải sửa MapEditorState (vi phạm
    // nguyên tắc đóng-mở). Nay mỗi tool tự lo phần xem trước của mình.
    //   gx, gy   : ô lưới đang trỏ
    //   cellRect : chính ô đó, đã đổi sang toạ độ thế giới
    virtual void renderGhost(int gx, int gy, Rectangle cellRect,
                             const CustomMapData& data) const = 0;
};
