#include "EditorBottomPanel.h"
#include "EditorBlockRegistry.h"
#include "EditorTextureCache.h"
#include <algorithm>

// =============================================================================
// Constructor
// =============================================================================

EditorBottomPanel::EditorBottomPanel()
    : categoryPanel_([this](const std::string& cat) {
          // Observer: khi category thay đổi → cập nhật blockVariantPanel
          if (cat != "Entities") {
              blockVariantPanel_.onCategoryChanged(cat);
          }
          // Chỉ đổi tool khi thực sự chuyển GIỮA đặt-block và đặt-entity.
          //
          // Trước đây dòng này gán vô điều kiện, nên đang chọn Erase mà mở một
          // category khác để tìm block là bị âm thầm đưa về Place — cú click kế
          // tiếp vẽ thay vì xoá. Người dùng đang xoá thì phải vẫn đang xoá.
          if (activeTool_ != EditorToolType::Erase) {
              activeTool_ = (cat == "Entities") ? EditorToolType::PlaceEntity
                                                : EditorToolType::Place;
          }
      }),
      blockVariantPanel_([](const std::string&) {
          // Block selected — MapEditorState đọc getSelectedBlockId()
      })
{}

void EditorBottomPanel::init() {
    EditorBlockRegistry::getInstance().init();
    categoryPanel_.init();
    // Auto-select first non-entity category
    blockVariantPanel_.onCategoryChanged(categoryPanel_.getSelectedCategory());
}

bool EditorBottomPanel::isEntityTab() const {
    return categoryPanel_.getSelectedCategory() == "Entities";
}

const std::string& EditorBottomPanel::getSelectedBlockId() const {
    return blockVariantPanel_.getSelectedBlockId();
}

void EditorBottomPanel::clearRequests() {
    reqSave_ = reqLoad_ = reqPlay_ = reqExit_ = reqUndo_ = reqRedo_ = false;
}

void EditorBottomPanel::updateErrorTimer(float dt) {
    if (errorTimer_ > 0.0f) errorTimer_ -= dt;
}

Rectangle EditorBottomPanel::getPanelRect(float screenW, float screenH) const {
    ui_.Refresh();
    float h = ui_.S(PANEL_HEIGHT);
    return { 0.0f, screenH - h, screenW, h };
}

// =============================================================================
// computeLayout — nguồn chân lý DUY NHẤT cho bố cục panel.
//
// Trước đây render() và handleInput() mỗi hàm tự tính lại 6 dòng y hệt nhau;
// chỉ cần sửa một hằng số ở một bên là vùng vẽ lệch khỏi vùng bấm.
//
// Mọi hằng số đều đi qua ui_.S() — xem ghi chú ở đầu EditorBottomPanel.h.
// =============================================================================
EditorBottomPanel::Layout EditorBottomPanel::computeLayout(float screenW, float screenH) const {
    ui_.Refresh();
    Layout L;

    const float panelH   = ui_.S(PANEL_HEIGHT);
    const float catW     = ui_.S(CAT_WIDTH);
    const float btnW     = ui_.S(BTN_WIDTH);
    const float btnH     = ui_.S(BTN_HEIGHT);
    const float pad      = ui_.S(BTN_PAD);
    const float actBtnW  = ui_.S(ACTION_BTN_W);
    const float gap      = ui_.S(4.0f);

    L.panel    = { 0.0f, screenH - panelH, screenW, panelH };
    L.category = { L.panel.x, L.panel.y, catW, L.panel.height };

    const float rightX      = L.panel.x + catW + gap;
    const float actionAreaW = (actBtnW + pad) * kActionCount + pad;
    const float toolAreaW   = btnW + pad * 2.0f;

    float variantW = screenW - rightX - toolAreaW - actionAreaW - gap;
    if (variantW < 0.0f) variantW = 0.0f;   // an toàn tuyệt đối cho BeginScissorMode

    L.variant  = { rightX, L.panel.y + ui_.S(2.0f), variantW, L.panel.height - ui_.S(4.0f) };
    L.iconSize = ui_.S(BlockVariantPanel::ICON_SIZE);

    const float btnX = rightX + variantW + pad;
    const float btnY = L.panel.y + (L.panel.height - btnH * 2.0f - ui_.S(6.0f)) / 2.0f;
    L.toolPlace = { btnX, btnY, btnW, btnH };
    L.toolErase = { btnX, btnY + btnH + ui_.S(6.0f), btnW, btnH };

    const float abX = btnX + btnW + pad;
    const float abY = L.panel.y + (L.panel.height - btnH) / 2.0f;
    for (int i = 0; i < kActionCount; ++i) {
        L.action[i] = { abX + (actBtnW + pad) * i, abY, actBtnW, btnH };
    }
    return L;
}

// =============================================================================
// Helper: draw button, returns true if clicked (handled in handleInput separately)
// =============================================================================

static void drawBtn(const char* label, Rectangle r, bool active) {
    Color bg = active ? Color{60, 120, 220, 255} : Color{45, 45, 65, 255};
    Color border = active ? Color{100, 160, 255, 255} : Color{80, 80, 110, 255};
    DrawRectangleRec(r, bg);
    DrawRectangleLinesEx(r, 1.5f, border);
    // Cỡ chữ bám theo chiều cao nút để không tràn khi bố cục co lại ở màn nhỏ.
    int fs = (int)(r.height * 0.36f);
    if (fs < 8) fs = 8;
    int tw = MeasureText(label, fs);
    DrawText(label, (int)(r.x + (r.width - tw) / 2), (int)(r.y + (r.height - fs) / 2), fs, WHITE);
}

// =============================================================================
// Render — CHỈ vẽ, không thay đổi state
// =============================================================================

void EditorBottomPanel::render(float screenW, float screenH) const {
    const Layout L = computeLayout(screenW, screenH);

    // Panel background
    DrawRectangleRec(L.panel, Color{12, 12, 22, 248});
    DrawRectangleLinesEx(L.panel, 1.5f, Color{60, 60, 90, 255});

    categoryPanel_.render(L.category);

    if (isEntityTab()) {
        entityPalette_.render(L.variant);
    } else {
        blockVariantPanel_.render(L.variant);
    }

    // --- Tool buttons ---
    bool placing = (activeTool_ == EditorToolType::Place || activeTool_ == EditorToolType::PlaceEntity);
    bool erasing = (activeTool_ == EditorToolType::Erase);
    drawBtn("Place", L.toolPlace, placing);
    drawBtn("Erase", L.toolErase, erasing);

    // --- Action buttons (thứ tự phải khớp handleInput) ---
    static const char* kActionLabels[kActionCount] = {
        "Save", "Load", "Play", "Exit", "Undo", "Redo"
    };
    for (int i = 0; i < kActionCount; ++i) {
        drawBtn(kActionLabels[i], L.action[i], false);
    }

    // --- Error message ---
    if (errorTimer_ > 0.0f && !errorMsg_.empty()) {
        unsigned char alpha = (unsigned char)(std::min(255.0f, errorTimer_ * 100.0f));
        Color ec = { 255, 80, 80, alpha };
        int fs = (int)ui_.S(24.0f);
        DrawText(errorMsg_.c_str(), (int)(L.panel.x + ui_.S(8.0f)),
                 (int)(L.panel.y - ui_.S(30.0f)), fs, ec);
    }
}

// =============================================================================
// HandleInput — Xử lý click, scroll trên panel
// =============================================================================

void EditorBottomPanel::handleInput(float screenW, float screenH) {
    const Layout L = computeLayout(screenW, screenH);

    categoryPanel_.handleInput(L.category);

    if (isEntityTab()) {
        entityPalette_.handleInput(L.variant);
    } else {
        blockVariantPanel_.handleInput(L.variant);
    }

    // --- Button clicks (left press only) ---
    if (!IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) return;
    Vector2 mp = GetMousePosition();

    // Tool buttons
    if (CheckCollisionPointRec(mp, L.toolPlace)) {
        activeTool_ = isEntityTab() ? EditorToolType::PlaceEntity : EditorToolType::Place;
        return;
    }
    if (CheckCollisionPointRec(mp, L.toolErase)) {
        activeTool_ = EditorToolType::Erase;
        return;
    }

    // Action buttons — thứ tự phải khớp kActionLabels trong render()
    bool* const reqs[kActionCount] = {
        &reqSave_, &reqLoad_, &reqPlay_, &reqExit_, &reqUndo_, &reqRedo_
    };
    for (int i = 0; i < kActionCount; ++i) {
        if (CheckCollisionPointRec(mp, L.action[i])) { *reqs[i] = true; return; }
    }
}
