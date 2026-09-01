#pragma once
#include "CategoryPanel.h"
#include "BlockVariantPanel.h"
#include "EntityPalette.h"
#include "EditorToolManager.h"
#include "UIScaler.h"
#include "raylib.h"
#include <string>

// =============================================================================
// EditorBottomPanel — Composite UI container phía dưới màn hình.
//
// Chứa:
//   - CategoryPanel (trái)  : chọn category (Solid, OneWay, Entities...)
//   - BlockVariantPanel     : hiển thị block icons khi không phải Entities tab
//   - EntityPalette         : hiển thị entity icons khi category = "Entities"
//   - Tool buttons          : [Place] [Erase]
//   - Action buttons        : [Save] [Load] [Play] [Exit] [Undo] [Redo]
//
// Design: Facade cho sub-components, tuân thủ 4-phase game loop rule.
//
// BỐ CỤC ĐỘC LẬP ĐỘ PHÂN GIẢI
// Các hằng số dưới đây là pixel trong KHUNG THIẾT KẾ 2560x1440 (xem UIScaler.h),
// không phải pixel màn hình. Trước đây chúng được dùng thẳng làm pixel thật, nên
// phần bố cục cố định luôn chiếm 1168px bất kể màn hình to nhỏ:
//   - ở cửa sổ 1280 (cỡ mặc định) vùng chọn block chỉ còn 112px, hẹp hơn MỘT icon
//   - ở màn hình <= 1168px thì vùng này ÂM -> BeginScissorMode âm và
//     CheckCollisionPointRec luôn false -> không chọn được block/entity nào cả.
// Nay mọi số đều đi qua ui_.S(), nên tỉ lệ giữ nguyên trên mọi độ phân giải.
// =============================================================================
class EditorBottomPanel {
public:
    static constexpr float PANEL_HEIGHT  = 340.0f;
    static constexpr float CAT_WIDTH     = CategoryPanel::WIDTH;
    static constexpr float BTN_WIDTH     = 140.0f;
    static constexpr float BTN_HEIGHT    = 60.0f;
    static constexpr float BTN_PAD       = 15.0f;
    static constexpr float ACTION_BTN_W  = 120.0f;
    // Save, Load, Play, Exit, Undo, Redo
    static constexpr int   kActionCount  = 6;

    EditorBottomPanel();

    // Khởi tạo registry + category panel
    void init();

    // --- 4-phase game loop ---
    void render(float screenW, float screenH) const;
    void handleInput(float screenW, float screenH);

    // --- Getters cho MapEditorState ---
    const std::string& getSelectedBlockId()    const;
    const std::string& getSelectedEntityType() const { return entityPalette_.getSelectedId(); }
    EditorToolType     getActiveTool()         const { return activeTool_; }

    bool requestSave()   const { return reqSave_; }
    bool requestLoad()   const { return reqLoad_; }
    bool requestPlay()   const { return reqPlay_; }
    bool requestExit()   const { return reqExit_; }
    bool requestUndo()   const { return reqUndo_; }
    bool requestRedo()   const { return reqRedo_; }
    void clearRequests();

    // --- Error display ---
    void  setErrorMessage(const std::string& msg) { errorMsg_ = msg; errorTimer_ = 3.0f; }
    void  updateErrorTimer(float dt);

    // --- Panel geometry ---
    Rectangle getPanelRect(float screenW, float screenH) const;

    // EntityPalette public access (cho MapEditorState ghost preview)
    const EntityPalette& getEntityPalette() const { return entityPalette_; }

private:
    CategoryPanel     categoryPanel_;
    BlockVariantPanel blockVariantPanel_;
    EntityPalette     entityPalette_;

    EditorToolType activeTool_ = EditorToolType::Place;

    bool reqSave_ = false;
    bool reqLoad_ = false;
    bool reqPlay_ = false;
    bool reqExit_ = false;
    bool reqUndo_ = false;
    bool reqRedo_ = false;

    std::string errorMsg_;
    float       errorTimer_ = 0.0f;

    mutable UIScaler ui_;   // mutable: render() là const nhưng vẫn phải đọc lại cỡ màn hình

    bool isEntityTab() const;

    // Bố cục đã quy đổi sang pixel màn hình. Tính MỘT lần rồi dùng chung cho cả
    // render lẫn handleInput — trước đây hai hàm chép y hệt 6 dòng tính toán,
    // và chỉ cần lệch một hằng số là vùng vẽ không còn khớp vùng bấm.
    struct Layout {
        Rectangle panel;
        Rectangle category;
        Rectangle variant;
        Rectangle toolPlace;
        Rectangle toolErase;
        Rectangle action[6];   // Save, Load, Play, Exit, Undo, Redo
        float     iconSize;
    };
    Layout computeLayout(float screenW, float screenH) const;
};
