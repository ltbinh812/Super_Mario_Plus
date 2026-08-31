#pragma once
#include "EntityDef.h"
#include "raylib.h"
#include <string>
#include <vector>

// =============================================================================
// EntityPalette — Renders và handles input cho danh sách entity trong editor.
//
// Tách ra khỏi EditorBottomPanel (Single Responsibility Principle).
// Dùng EditorTextureCache để lấy texture (Flyweight).
//
// Flow: CategoryPanel chọn "Entities" → EditorBottomPanel delegate render/input vào đây.
// =============================================================================
class EntityPalette {
public:
    static constexpr float ICON_SIZE    = 128.0f;
    static constexpr float ICON_PAD     = 15.0f;
    static constexpr float LABEL_HEIGHT = 28.0f;

    EntityPalette();

    // Render danh sách entity icons
    void render(Rectangle panelRect) const;

    // Xử lý click chọn entity
    void handleInput(Rectangle panelRect);

    const std::string& getSelectedId() const { return selectedId_; }

    // Trả về tất cả paths cần load texture (cho EditorTextureCache)
    std::vector<std::string> getAllTexturePaths() const;

    // Lấy EntityDef theo id (dùng cho ghost preview và map rendering)
    const EntityDef* getEntityDef(const std::string& id) const;

private:
    std::vector<EntityDef> entities_;
    std::string selectedId_;
    int scrollOffsetX_ = 0;

    void initEntityList();
    void drawEntityIcon(const EntityDef& def, Rectangle dest, bool selected) const;
};
