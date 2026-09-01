#pragma once
#include "GameState.h"
#include "TileMap.h"
#include "CustomMapData.h"
#include "EditorCamera.h"
#include "EditorMapResizer.h"
#include "EditorBottomPanel.h"
#include "EditorToolManager.h"
#include "UndoRedoStack.h"
#include "CustomMapSerializer.h"
#include "EditorSaveLoadUI.h"
#include <memory>
#include <string>

// =============================================================================
// MapEditorState — GameState cho chế độ thiết kế map (Maker Mode).
//
// Tuân thủ GEMINI.md: 4 giai đoạn game loop tách biệt rõ ràng.
// Tuân thủ OOP enforcer: 1 class / 1 file.
//
// Kiến trúc:
//   HandleInput → đọc raw mouse/keyboard → intent flags
//   Process     → apply intent vào data, xử lý tools, save/load
//   Update(dt)  → update camera, rebuild TileMap canvas nếu dirty
//   Render      → vẽ map, grid, ghost preview, UI panels
// =============================================================================
class MapEditorState : public GameState {
public:
    MapEditorState();
    ~MapEditorState() override;

    void HandleInput() override;
    void Process()     override;
    void Update(float dt) override;
    void Render(float alpha) const override;

private:
    // --- Core data ---
    CustomMapData    mapData_;
    TileMap          tileMap_;

    // --- Editor components ---
    EditorCamera      editorCamera_;
    EditorMapResizer  resizer_;
    EditorBottomPanel bottomPanel_;
    EditorToolManager toolManager_;
    UndoRedoStack     undoRedo_;
    EditorSaveLoadUI  saveLoadUI_;

    // --- Intent flags (set trong HandleInput, consumed trong Process) ---
    int     pendingGridX_   = -1;
    int     pendingGridY_   = -1;
    bool    mouseOnMap_     = false;
    bool    mouseOnPanel_   = false;
    bool    leftPress_      = false;
    bool    leftDown_       = false;
    bool    leftRelease_    = false;
    bool    rightPress_     = false;
    bool    rightDown_      = false;
    Vector2 mouseWorldPos_  = {0, 0};

    // --- State flags ---
    bool mapDirty_          = true;
    bool showSaveLoadUI_ = false;
    SaveLoadMode saveLoadMode_ = SaveLoadMode::Save;

    // --- Back Button ---
    Texture2D backBtnTex_;
    Texture2D backBtnPressTex_;
    Rectangle backBtnRect_;
    bool isBackBtnHovered_ = false;
    bool isBackBtnPressed_ = false;

    // --- Helpers ---
    void handleSaveLoadSlotAction(int slot);
    void handleSave(int slot);
    void handleLoad(int slot);      // nạp slot vào editor ĐỂ SỬA
    void handleTestPlay(int slot);  // chơi thử map đang sửa
    bool validateMapBeforeAction(const std::string& action);

    // --- Render helpers (const — không thay đổi state) ---
    void drawGrid()            const;
    void drawEntityIcons()     const;
    void drawGhostPreview()    const;
    void drawCoordinateAxes()  const;

    float worldTileSize() const;
};
