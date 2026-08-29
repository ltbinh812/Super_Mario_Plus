#include "MapEditorState.h"
#include "EditorBlockRegistry.h"
#include "EditorTextureCache.h"
#include "CustomMapSerializer.h"
#include "command/StateCommands.h"
#include <raylib.h>
#include <iostream>
#include <algorithm>
#include <cmath>

// =============================================================================
// Constructor / Destructor
// =============================================================================

MapEditorState::MapEditorState() {
    // 1. Khởi tạo registry (Singleton, idempotent)
    EditorBlockRegistry::getInstance().init();

    // 2. Khởi tạo bottom panel (categories, entity palette)
    bottomPanel_.init();

    // 3. Load toàn bộ textures vào cache (Flyweight)
    //    Thu thập tất cả paths từ block defs + entity palette
    std::vector<std::string> texturePaths;
    for (const auto& cat : EditorBlockRegistry::getInstance().getOrderedCategories()) {
        for (const auto& blockId : EditorBlockRegistry::getInstance().getAllInCategory(cat)) {
            const auto& def = EditorBlockRegistry::getInstance().get(blockId);
            if (!def.tilesetPath.empty()) {
                texturePaths.push_back(def.tilesetPath);
            }
        }
    }
    // Entity palette paths
    for (const auto& p : bottomPanel_.getEntityPalette().getAllTexturePaths()) {
        texturePaths.push_back(p);
    }
    // Deduplicate
    std::sort(texturePaths.begin(), texturePaths.end());
    texturePaths.erase(std::unique(texturePaths.begin(), texturePaths.end()), texturePaths.end());
    // Load all
    EditorTextureCache::getInstance().loadAll(texturePaths);

    // 4. Default map: 20x15 tiles
    mapData_.width  = 20;
    mapData_.height = 15;
    mapData_.name   = "New Map";

    // 5. Center camera vào giữa map
    float mapCenterX = (mapData_.width  * worldTileSize()) / 2.0f;
    float mapCenterY = (mapData_.height * worldTileSize()) / 2.0f;
    editorCamera_.panTo(mapCenterX, mapCenterY);

    // 6. Init save/load UI
    saveLoadUI_.init();

    // 7. Init tool manager
    toolManager_.setTool(EditorToolType::Place,
                         &bottomPanel_.getSelectedBlockId(),
                         &bottomPanel_.getSelectedEntityType());

    mapDirty_ = true;
    
    // 8. Back button
    backBtnTex_ = LoadTexture("assets/UI_screens/menu_btn_back.png");
    backBtnPressTex_ = LoadTexture("assets/UI_screens/menu_btn_back_press.png");
}

MapEditorState::~MapEditorState() {
    EditorTextureCache::getInstance().unloadAll();
    UnloadTexture(backBtnTex_);
    UnloadTexture(backBtnPressTex_);
}

float MapEditorState::worldTileSize() const {
    return (float)mapData_.tileSize * 2.0f;  // 16 * 2 = 32px
}

// =============================================================================
// HandleInput — Chỉ đọc raw input, ghi vào intent flags
// =============================================================================

void MapEditorState::HandleInput() {
    // Khi save/load UI mở → không đọc map input
    if (showSaveLoadUI_) return;

    Vector2 mouseScreen = GetMousePosition();
    
    // Back button hover effect
    float baseSize = 48.0f;
    float hoverSize = 56.0f;
    
    // Fixed hitbox based on hover size to prevent jitter
    Rectangle hitBox = { 10.0f, 10.0f, hoverSize, hoverSize };
    isBackBtnHovered_ = CheckCollisionPointRec(mouseScreen, hitBox);
    
    if (backBtnTex_.id != 0) {
        float btnSize = isBackBtnHovered_ ? hoverSize : baseSize;
        float offset = (hoverSize - btnSize) / 2.0f;
        backBtnRect_ = { 10.0f + offset, 10.0f + offset, btnSize, btnSize };
        
        if (isBackBtnHovered_ && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            isBackBtnPressed_ = true;
        }
    }

    // Tính world position từ screen
    mouseWorldPos_ = editorCamera_.screenToWorld(mouseScreen);

    // Tính grid coords
    float wts  = worldTileSize();
    pendingGridX_ = (int)std::floor(mouseWorldPos_.x / wts);
    pendingGridY_ = (int)std::floor(mouseWorldPos_.y / wts);

    // Phân biệt chuột trên map hay panel
    float screenH  = (float)GetScreenHeight();
    float panelTop = screenH - EditorBottomPanel::PANEL_HEIGHT;
    mouseOnPanel_  = (mouseScreen.y >= panelTop);
    mouseOnMap_    = !mouseOnPanel_ && mapData_.inBounds(pendingGridX_, pendingGridY_);

    // Đọc mouse buttons
    leftPress_   = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    leftDown_    = IsMouseButtonDown(MOUSE_LEFT_BUTTON);
    leftRelease_ = IsMouseButtonReleased(MOUSE_LEFT_BUTTON);
    rightPress_  = IsMouseButtonPressed(MOUSE_RIGHT_BUTTON);
    rightDown_   = IsMouseButtonDown(MOUSE_RIGHT_BUTTON);

    // Phím tắt
    // (Ctrl+Z và Ctrl+Y được xử lý trong Process)
    
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        // We will process the click in Process(), then reset
    }
}

// =============================================================================
// Process — Apply intent vào data, không vẽ
// =============================================================================

void MapEditorState::Process() {
    float wts = worldTileSize();

    // --- Back button action ---
    if (isBackBtnHovered_ && isBackBtnPressed_ && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        PushStateCommand(std::make_unique<PopStateCommand>());
        isBackBtnPressed_ = false;
        return;
    }
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        isBackBtnPressed_ = false;
    }

    // --- Bottom panel ---
    bottomPanel_.clearRequests();
    if (!showSaveLoadUI_) {
        bottomPanel_.handleInput((float)GetScreenWidth(), (float)GetScreenHeight());
    }

    // Sync tool type từ panel sang toolManager (Strategy pattern)
    EditorToolType panelTool = bottomPanel_.getActiveTool();
    if (panelTool != toolManager_.getActiveTool()) {
        toolManager_.setTool(panelTool,
                             &bottomPanel_.getSelectedBlockId(),
                             &bottomPanel_.getSelectedEntityType());
    }

    // --- Save button ---
    if (bottomPanel_.requestSave() && !showSaveLoadUI_) {
        saveLoadUI_.init();  // refresh slot status
        showSaveLoadUI_   = true;
        saveLoadModeIsSave_ = true;
    }
    // --- Load button ---
    if (bottomPanel_.requestLoad() && !showSaveLoadUI_) {
        saveLoadUI_.init();
        showSaveLoadUI_   = true;
        saveLoadModeIsSave_ = false;
    }
    // --- Exit button → quay về menu ---
    if (bottomPanel_.requestExit()) {
        PushStateCommand(std::make_unique<PopStateCommand>());
        return;
    }

    // --- Undo / Redo ---
    bool doUndo = bottomPanel_.requestUndo() ||
                  (IsKeyPressed(KEY_Z) && IsKeyDown(KEY_LEFT_CONTROL));
    bool doRedo = bottomPanel_.requestRedo() ||
                  (IsKeyPressed(KEY_Y) && IsKeyDown(KEY_LEFT_CONTROL));

    if (doUndo && undoRedo_.canUndo()) {
        undoRedo_.undo(mapData_);
        mapDirty_ = true;
    }
    if (doRedo && undoRedo_.canRedo()) {
        undoRedo_.redo(mapData_);
        mapDirty_ = true;
    }

    // --- Save/Load UI popup ---
    if (showSaveLoadUI_) {
        saveLoadUI_.handleInput(
            (float)GetScreenWidth(), (float)GetScreenHeight(),
            [this](int slot) { handleSaveLoadSlotAction(slot); },
            [this]() { showSaveLoadUI_ = false; }
        );
        // Escape để đóng popup
        if (IsKeyPressed(KEY_ESCAPE)) {
            showSaveLoadUI_ = false;
        }
        return;  // chặn map interaction khi popup mở
    }

    // --- Escape → exit (khi popup không mở) ---
    if (IsKeyPressed(KEY_ESCAPE)) {
        PushStateCommand(std::make_unique<PopStateCommand>());
        return;
    }

    // --- Map resize handles ---
    bool resized = resizer_.process(mapData_, wts, mouseWorldPos_);
    if (resized) mapDirty_ = true;

    // --- Tool dispatch (chỉ khi chuột trên map) ---
    if (mouseOnMap_) {
        bool willChange = (leftPress_ || rightPress_);
        if (willChange) undoRedo_.pushUndo(mapData_);

        bool changed = toolManager_.dispatch(
            pendingGridX_, pendingGridY_,
            leftPress_  && !mouseOnPanel_,
            leftDown_   && !mouseOnPanel_,
            leftRelease_,
            rightPress_ && !mouseOnPanel_,
            rightDown_  && !mouseOnPanel_,
            mapData_);

        if (changed) mapDirty_ = true;
    }
}

// =============================================================================
// Update — Cập nhật camera, rebuild canvas nếu dirty
// =============================================================================

void MapEditorState::Update(float dt) {
    bottomPanel_.updateErrorTimer(dt);

    int mapW = (int)(mapData_.width  * worldTileSize());
    int mapH = (int)(mapData_.height * worldTileSize());
    editorCamera_.update(dt, mapW, mapH, mouseOnPanel_);

    if (mapDirty_) {
        tileMap_.LoadCustomMap(mapData_);
        mapDirty_ = false;
    }
}

// =============================================================================
// Render — CHỈ vẽ, không thay đổi state
// =============================================================================

void MapEditorState::Render(float /*alpha*/) const {
    ClearBackground(Color{18, 18, 28, 255});

    // --- World space (camera transform) ---
    editorCamera_.beginMode();
        tileMap_.Draw();
        drawCoordinateAxes();
        drawGrid();
        drawEntityIcons();
        drawGhostPreview();
        resizer_.render(mapData_, worldTileSize());
    editorCamera_.endMode();

    // --- Screen space UI ---
    bottomPanel_.render((float)GetScreenWidth(), (float)GetScreenHeight());

    // --- Back button ---
    if (backBtnTex_.id != 0) {
        Texture2D tex = isBackBtnPressed_ ? backBtnPressTex_ : backBtnTex_;
        if (tex.id == 0) tex = backBtnTex_;
        Rectangle src = { 0, 0, (float)tex.width, (float)tex.height };
        DrawTexturePro(tex, src, backBtnRect_, {0,0}, 0.0f, WHITE);
    }

    // Info overlay (Fixed Y position based on max button size)
    float infoY = 10.0f + 56.0f + 10.0f;
    DrawText(mapData_.name.c_str(), 10, (int)infoY, 16, Color{200, 200, 220, 210});
    {
        char buf[48];
        snprintf(buf, sizeof(buf), "Zoom: %.2fx", editorCamera_.getZoom());
        DrawText(buf, 10, (int)infoY + 22, 12, Color{150, 150, 180, 180});
    }
    if (mouseOnMap_) {
        char buf[48];
        snprintf(buf, sizeof(buf), "(%d, %d)", pendingGridX_, pendingGridY_);
        DrawText(buf, 10, (int)infoY + 38, 12, Color{130, 200, 130, 200});
    }

    // Save/Load popup overlay
    if (showSaveLoadUI_) {
        saveLoadUI_.render((float)GetScreenWidth(), (float)GetScreenHeight(), saveLoadModeIsSave_);
    }
}

// =============================================================================
// Render Helpers
// =============================================================================

void MapEditorState::drawGrid() const {
    float wts   = worldTileSize();
    float mapW  = mapData_.width  * wts;
    float mapH  = mapData_.height * wts;
    Color gridC = Color{70, 70, 100, 55};

    for (int x = 0; x <= mapData_.width; ++x)
        DrawLineEx({x * wts, 0}, {x * wts, mapH}, 0.5f, gridC);
    for (int y = 0; y <= mapData_.height; ++y)
        DrawLineEx({0, y * wts}, {mapW, y * wts}, 0.5f, gridC);

    // Map border
    DrawRectangleLinesEx({0, 0, mapW, mapH}, 2.0f, Color{100, 120, 180, 220});
}

void MapEditorState::drawCoordinateAxes() const {
    float wts   = worldTileSize();
    float mapW  = mapData_.width  * wts;
    float mapH  = mapData_.height * wts;
    Color axisX = Color{60, 200, 100, 160};
    Color axisY = Color{200, 80, 80, 160};

    DrawLineEx({-wts * 0.5f, 0},    {mapW + wts * 0.5f, 0},    1.5f, axisX);
    DrawLineEx({0, -wts * 0.5f},    {0, mapH + wts * 0.5f},    1.5f, axisY);

    for (int x = 0; x <= mapData_.width; x += 5) {
        char buf[8]; snprintf(buf, sizeof(buf), "%d", x);
        DrawText(buf, (int)(x * wts + 2), -14, 9, axisX);
    }
    for (int y = 0; y <= mapData_.height; y += 5) {
        char buf[8]; snprintf(buf, sizeof(buf), "%d", y);
        DrawText(buf, -22, (int)(y * wts + 2), 9, axisY);
    }
}

void MapEditorState::drawEntityIcons() const {
    float wts = worldTileSize();
    auto& palette = bottomPanel_.getEntityPalette();

    for (const auto& e : mapData_.entities) {
        float x = e.gridX * wts;
        float y = e.gridY * wts;
        Rectangle dest = { x, y, wts, wts };

        const EntityDef* def = palette.getEntityDef(e.type);
        bool drawn = false;

        if (def && !def->texturePath.empty()) {
            const Texture2D& tex = EditorTextureCache::getInstance().getOrDefault(def->texturePath);
            if (tex.id != 0) {
                DrawTexturePro(tex, def->uv, dest, {0, 0}, 0.0f, WHITE);
                drawn = true;
            }
        }

        if (!drawn) {
            // Fallback: colored rect + label
            Color c = def ? def->fallbackColor : Color{255, 200, 50, 220};
            DrawRectangleRec(dest, Color{c.r, c.g, c.b, 70});
            DrawRectangleLinesEx(dest, 2.0f, c);
            std::string lbl = e.type.substr(0, std::min((int)e.type.size(), 5));
            DrawText(lbl.c_str(), (int)(x + 2), (int)(y + (wts - 10) / 2), 10, c);
        }
    }
}

void MapEditorState::drawGhostPreview() const {
    if (!mouseOnMap_) return;

    float wts  = worldTileSize();
    Rectangle destRect = {
        pendingGridX_ * wts,
        pendingGridY_ * wts,
        wts, wts
    };

    EditorToolType tool = toolManager_.getActiveTool();

    if (tool == EditorToolType::Erase) {
        // Erase ghost: red tinted
        DrawRectangleRec(destRect, Color{255, 60, 60, 50});
        DrawRectangleLinesEx(destRect, 2.0f, Color{255, 80, 80, 180});
        return;
    }

    if (tool == EditorToolType::PlaceEntity) {
        const std::string& entityId = bottomPanel_.getSelectedEntityType();
        const EntityDef* def = bottomPanel_.getEntityPalette().getEntityDef(entityId);
        bool drawn = false;

        if (def && !def->texturePath.empty()) {
            const Texture2D& tex = EditorTextureCache::getInstance().getOrDefault(def->texturePath);
            if (tex.id != 0) {
                float scaleX = wts / std::abs(def->uv.width);
                float scaleY = wts / std::abs(def->uv.height);
                float minScale = std::min(scaleX, scaleY);
                
                float newW = std::abs(def->uv.width) * minScale;
                float newH = std::abs(def->uv.height) * minScale;
                float offX = (wts - newW) / 2.0f;
                float offY = (wts - newH) / 2.0f;
                
                Rectangle trueDest = {
                    destRect.x + offX,
                    destRect.y + offY,
                    newW, newH
                };
                DrawTexturePro(tex, def->uv, trueDest, {0, 0}, 0.0f, Color{255, 255, 255, 130});
                drawn = true;
            }
        }
        if (!drawn && def) {
            Color gc = {def->fallbackColor.r, def->fallbackColor.g, def->fallbackColor.b, 100};
            DrawRectangleRec(destRect, gc);
        }
        DrawRectangleLinesEx(destRect, 2.0f, Color{255, 255, 255, 130});
        return;
    }

    // Place block ghost
    const std::string& blockId = bottomPanel_.getSelectedBlockId();
    if (blockId.empty()) return;

    auto& reg = EditorBlockRegistry::getInstance();
    if (!reg.has(blockId)) return;

    const auto& def = reg.get(blockId);
    bool drawn = false;

    if (!def.tilesetPath.empty()) {
        const Texture2D& tex = EditorTextureCache::getInstance().getOrDefault(def.tilesetPath);
        if (tex.id != 0) {
            float scaleX = wts / std::abs(def.uv.width);
            float scaleY = wts / std::abs(def.uv.height);
            float minScale = std::min(scaleX, scaleY);
            
            float newW = std::abs(def.uv.width) * minScale;
            float newH = std::abs(def.uv.height) * minScale;
            float offX = (wts - newW) / 2.0f;
            float offY = (wts - newH) / 2.0f;
            
            Rectangle trueDest = {
                (float)(pendingGridX_ * wts) + offX,
                (float)(pendingGridY_ * wts) + offY,
                newW, newH
            };
            DrawTexturePro(tex, def.uv, trueDest, {0, 0}, 0.0f, Color{255, 255, 255, 130});
            drawn = true;
        }
    }
    if (!drawn) {
        Color gc = {def.fallbackColor.r, def.fallbackColor.g, def.fallbackColor.b, 100};
        DrawRectangleRec(destRect, gc);
    }
    DrawRectangleLinesEx(destRect, 2.0f, Color{255, 255, 255, 130});
}

// =============================================================================
// Save / Load
// =============================================================================

void MapEditorState::handleSaveLoadSlotAction(int slot) {
    if (saveLoadModeIsSave_) {
        if (validateMapBeforeAction("Save")) {
            handleSave(slot);
            showSaveLoadUI_ = false;
        }
    } else {
        handleLoad(slot);
        showSaveLoadUI_ = false;
    }
}

void MapEditorState::handleSave(int slot) {
    bool ok = CustomMapSerializer::save(mapData_, slot);
    if (!ok) {
        bottomPanel_.setErrorMessage("Save failed! Check console.");
    } else {
        std::cout << "[MapEditorState] Saved to slot " << slot << "\n";
        // Export to LDtk for BaseLevelState to read
        CustomMapSerializer::exportToLDtk(mapData_, slot);
    }
}

void MapEditorState::handleLoad(int slot) {
    if (!CustomMapSerializer::slotExists(slot)) {
        bottomPanel_.setErrorMessage("Slot " + std::to_string(slot) + " is empty.");
        return;
    }
    undoRedo_.pushUndo(mapData_);
    mapData_  = CustomMapSerializer::load(slot);
    mapDirty_ = true;
}

// =============================================================================
// Validation
// =============================================================================

bool MapEditorState::validateMapBeforeAction(const std::string& action) {
    int spawns = mapData_.countPlayerSpawns();
    if (spawns < 1) {
        bottomPanel_.setErrorMessage(
            action + " failed: need at least 1 PlayerSpawn (found " +
            std::to_string(spawns) + ")");
        return false;
    }
    return true;
}
