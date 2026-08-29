#pragma once
#include "CategoryPanel.h"
#include "BlockVariantPanel.h"
#include "EntityPalette.h"
#include "EditorToolManager.h"
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
//   - Action buttons        : [Save] [Load] [Exit] [Undo] [Redo]
//
// Design: Facade cho sub-components, tuân thủ 4-phase game loop rule.
// =============================================================================
class EditorBottomPanel {
public:
    static constexpr float PANEL_HEIGHT  = 340.0f;
    static constexpr float CAT_WIDTH     = CategoryPanel::WIDTH;
    static constexpr float BTN_WIDTH     = 140.0f;
    static constexpr float BTN_HEIGHT    = 60.0f;
    static constexpr float BTN_PAD       = 15.0f;
    static constexpr float ACTION_BTN_W  = 120.0f;

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

    // --- Action request flags (reset mỗi frame) ---
    bool requestSave()   const { return reqSave_; }
    bool requestLoad()   const { return reqLoad_; }
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
    bool reqExit_ = false;
    bool reqUndo_ = false;
    bool reqRedo_ = false;

    std::string errorMsg_;
    float       errorTimer_ = 0.0f;

    bool isEntityTab() const;
};
