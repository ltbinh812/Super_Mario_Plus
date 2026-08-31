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
          // Khi chuyển sang Entities tab → switch tool về PlaceEntity
          // Khi chuyển sang tab khác → switch tool về Place
          activeTool_ = (cat == "Entities") ? EditorToolType::PlaceEntity : EditorToolType::Place;
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
    reqSave_ = reqLoad_ = reqExit_ = reqUndo_ = reqRedo_ = false;
}

void EditorBottomPanel::updateErrorTimer(float dt) {
    if (errorTimer_ > 0.0f) errorTimer_ -= dt;
}

Rectangle EditorBottomPanel::getPanelRect(float screenW, float screenH) const {
    return { 0.0f, screenH - PANEL_HEIGHT, screenW, PANEL_HEIGHT };
}

// =============================================================================
// Helper: draw button, returns true if clicked (handled in handleInput separately)
// =============================================================================

static void drawBtn(const char* label, Rectangle r, bool active) {
    Color bg = active ? Color{60, 120, 220, 255} : Color{45, 45, 65, 255};
    Color border = active ? Color{100, 160, 255, 255} : Color{80, 80, 110, 255};
    DrawRectangleRec(r, bg);
    DrawRectangleLinesEx(r, 1.5f, border);
    int tw = MeasureText(label, 22);
    DrawText(label, (int)(r.x + (r.width - tw) / 2), (int)(r.y + (r.height - 22) / 2), 22, WHITE);
}

// =============================================================================
// Render — CHỈ vẽ, không thay đổi state
// =============================================================================

void EditorBottomPanel::render(float screenW, float screenH) const {
    Rectangle panel = getPanelRect(screenW, screenH);

    // Panel background
    DrawRectangleRec(panel, Color{12, 12, 22, 248});
    DrawRectangleLinesEx(panel, 1.5f, Color{60, 60, 90, 255});

    // --- Left: CategoryPanel ---
    Rectangle catRect = { panel.x, panel.y, CAT_WIDTH, panel.height };
    categoryPanel_.render(catRect);

    // --- Center: Block/Entity variant area ---
    float rightX      = panel.x + CAT_WIDTH + 4.0f;
    float actionAreaW = (ACTION_BTN_W + BTN_PAD) * 5 + BTN_PAD;
    float toolAreaW   = BTN_WIDTH + BTN_PAD * 2;
    float variantW    = screenW - rightX - toolAreaW - actionAreaW - 4.0f;
    Rectangle variantRect = { rightX, panel.y + 2.0f, variantW, panel.height - 4.0f };

    if (isEntityTab()) {
        entityPalette_.render(variantRect);
    } else {
        blockVariantPanel_.render(variantRect);
    }

    // --- Tool buttons ---
    float btnX = rightX + variantW + BTN_PAD;
    float btnY = panel.y + (panel.height - BTN_HEIGHT * 2 - 6.0f) / 2.0f;
    bool placing = (activeTool_ == EditorToolType::Place || activeTool_ == EditorToolType::PlaceEntity);
    bool erasing = (activeTool_ == EditorToolType::Erase);
    drawBtn("Place", { btnX, btnY,               BTN_WIDTH, BTN_HEIGHT }, placing);
    drawBtn("Erase", { btnX, btnY + BTN_HEIGHT + 6, BTN_WIDTH, BTN_HEIGHT }, erasing);

    // --- Action buttons ---
    float abX = btnX + BTN_WIDTH + BTN_PAD;
    float abY = panel.y + (panel.height - BTN_HEIGHT) / 2.0f;
    drawBtn("Save", { abX,                              abY, ACTION_BTN_W, BTN_HEIGHT }, false);
    drawBtn("Load", { abX + (ACTION_BTN_W + BTN_PAD),  abY, ACTION_BTN_W, BTN_HEIGHT }, false);
    drawBtn("Exit", { abX + (ACTION_BTN_W + BTN_PAD)*2,abY, ACTION_BTN_W, BTN_HEIGHT }, false);
    drawBtn("Undo", { abX + (ACTION_BTN_W + BTN_PAD)*3,abY, ACTION_BTN_W, BTN_HEIGHT }, false);
    drawBtn("Redo", { abX + (ACTION_BTN_W + BTN_PAD)*4,abY, ACTION_BTN_W, BTN_HEIGHT }, false);

    // --- Error message ---
    if (errorTimer_ > 0.0f && !errorMsg_.empty()) {
        unsigned char alpha = (unsigned char)(std::min(255.0f, errorTimer_ * 100.0f));
        Color ec = { 255, 80, 80, alpha };
        DrawText(errorMsg_.c_str(), (int)(panel.x + 8), (int)(panel.y - 30), 24, ec);
    }
}

// =============================================================================
// HandleInput — Xử lý click, scroll trên panel
// =============================================================================

void EditorBottomPanel::handleInput(float screenW, float screenH) {
    Rectangle panel = getPanelRect(screenW, screenH);

    // --- Category Panel ---
    categoryPanel_.handleInput({ panel.x, panel.y, CAT_WIDTH, panel.height });

    // --- Variant area ---
    float rightX      = panel.x + CAT_WIDTH + 4.0f;
    float actionAreaW = (ACTION_BTN_W + BTN_PAD) * 5 + BTN_PAD;
    float toolAreaW   = BTN_WIDTH + BTN_PAD * 2;
    float variantW    = screenW - rightX - toolAreaW - actionAreaW - 4.0f;
    Rectangle variantRect = { rightX, panel.y + 2.0f, variantW, panel.height - 4.0f };

    if (isEntityTab()) {
        entityPalette_.handleInput(variantRect);
    } else {
        blockVariantPanel_.handleInput(variantRect);
    }

    // --- Button clicks (left press only) ---
    if (!IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) return;
    Vector2 mp = GetMousePosition();

    float btnX = rightX + variantW + BTN_PAD;
    float btnY = panel.y + (panel.height - BTN_HEIGHT * 2 - 6.0f) / 2.0f;

    // Tool buttons
    if (CheckCollisionPointRec(mp, { btnX, btnY, BTN_WIDTH, BTN_HEIGHT })) {
        activeTool_ = isEntityTab() ? EditorToolType::PlaceEntity : EditorToolType::Place;
        return;
    }
    if (CheckCollisionPointRec(mp, { btnX, btnY + BTN_HEIGHT + 6, BTN_WIDTH, BTN_HEIGHT })) {
        activeTool_ = EditorToolType::Erase;
        return;
    }

    // Action buttons
    float abX = btnX + BTN_WIDTH + BTN_PAD;
    float abY = panel.y + (panel.height - BTN_HEIGHT) / 2.0f;

    if (CheckCollisionPointRec(mp, { abX,                               abY, ACTION_BTN_W, BTN_HEIGHT })) { reqSave_ = true; return; }
    if (CheckCollisionPointRec(mp, { abX + (ACTION_BTN_W + BTN_PAD),   abY, ACTION_BTN_W, BTN_HEIGHT })) { reqLoad_ = true; return; }
    if (CheckCollisionPointRec(mp, { abX + (ACTION_BTN_W + BTN_PAD)*2, abY, ACTION_BTN_W, BTN_HEIGHT })) { reqExit_ = true; return; }
    if (CheckCollisionPointRec(mp, { abX + (ACTION_BTN_W + BTN_PAD)*3, abY, ACTION_BTN_W, BTN_HEIGHT })) { reqUndo_ = true; return; }
    if (CheckCollisionPointRec(mp, { abX + (ACTION_BTN_W + BTN_PAD)*4, abY, ACTION_BTN_W, BTN_HEIGHT })) { reqRedo_ = true; return; }
}
