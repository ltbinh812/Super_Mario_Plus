#include "EntityPalette.h"
#include "EditorTextureCache.h"
#include <raylib.h>
#include <algorithm>

// =============================================================================
// Catalog entity từ phân tích world01-06.ldtk:
//
// Tileset chính: assets/maps/item/a.png (64x64px mỗi entity trong file)
//   Coin      : x=256, y=0
//   Boom      : x=128, y=0
//   Door      : x=320, y=0
//   Flag      : x=576, y=0
//   Key       : x=704, y=0
//   LuckyBlock: x=64,  y=0
//   Spring_down : x=896,  y=0
//   Spring_right: x=960,  y=0
//   Spring_left : x=1024, y=0
//   Spring_up   : x=1088, y=0
//
// Tileset phụ:
//   Chest_boss  : assets/maps/item/chest_boss.png   {0,0,16,16}
//   Chest_normal: assets/maps/item/chest_normal.png {0,0,16,16}
//   Buff        : assets/maps/item/buff.png          {16,16,176,160}
//
// PlayerSpawn (Starting_position trong LDtk): Không có tile → fallback màu đỏ
// =============================================================================

EntityPalette::EntityPalette() {
    initEntityList();
    if (!entities_.empty()) {
        selectedId_ = entities_.front().id;
    }
}

void EntityPalette::initEntityList() {
    // ITEM/A.PNG — source of truth từ LDtk entity defs
    const std::string ITEM_A = "assets/maps/item/a.png";

    entities_ = {
        // PlayerSpawn — không có tile trong LDtk → fallback màu
        { "PlayerSpawn",  "Player Spawn",  "",                                      {0,0,0,0},         {100, 255, 120, 255} },
        // Coin — tilesetUid=208/321 (item/a.png), rect x=256 y=0 w=64 h=64
        { "Coin",         "Coin",          ITEM_A,                                  {256, 0, 64, 64},  {255, 220, 0,   255} },
        // LuckyBlock — item/a.png x=64 y=0
        { "Luckyblock",   "Lucky Block",   ITEM_A,                                  {64,  0, 64, 64},  {255, 140, 0,   255} },
        // Chest Normal — chest_normal.png
        { "Chest_normal", "Chest Normal",  "assets/maps/item/chest_normal.png",     {0, 0, 32, 32},  {150, 100, 50,  255} },
        // Chest Boss Key — chest_boss.png
        { "Chest_boss",   "Chest Boss Key","assets/maps/item/chest_boss.png",       {0, 0, 32, 32},  {200, 150, 50,  255} },
        // Flag — item/a.png x=576 y=0
        { "Flag",         "Flag",          ITEM_A,                                  {512, 0, 64, 64},  {0,   200, 50,  255} },
        // Door — item/a.png x=320 y=0
        { "Door",         "Door",          ITEM_A,                                  {320, 0, 64, 64},  {100, 50,  20,  255} },
        // Key — item/a.png x=704 y=0
        { "Key",          "Key",           ITEM_A,                                  {640, 0, 64, 64},  {200, 200, 50,  255} },
        // Boom — item/a.png x=128 y=0
        { "Boom",         "Bomb",          ITEM_A,                                  {128, 0, 64, 64},  {80,  80,  80,  255} },
        // Buff — buff.png, first frame at (16,16,176,160) nhưng quá lớn → dùng fallback
        { "Buff",         "Buff",          "assets/maps/item/buff.png",             {416, 16, 176, 176}, {255, 100, 200, 255} },
        // Spring variants — item/a.png
        { "Spring_up",    "Spring Up",     ITEM_A,                                  {1088, 0, 64, 64},  {50,  200, 80,  255} },
        { "Spring_down",  "Spring Down",   ITEM_A,                                  {896, 0, 64, 64},  {50,  200, 80,  255} },
        { "Spring_left",  "Spring Left",   ITEM_A,                                  {1024,0, 64, 64},  {50,  200, 80,  255} },
        { "Spring_right", "Spring Right",  ITEM_A,                                  {960, 0, 64, 64},  {50,  200, 80,  255} },
    };
}

std::vector<std::string> EntityPalette::getAllTexturePaths() const {
    std::vector<std::string> paths;
    for (const auto& e : entities_) {
        if (!e.texturePath.empty()) {
            bool found = false;
            for (const auto& p : paths) {
                if (p == e.texturePath) { found = true; break; }
            }
            if (!found) paths.push_back(e.texturePath);
        }
    }
    return paths;
}

const EntityDef* EntityPalette::getEntityDef(const std::string& id) const {
    for (const auto& e : entities_) {
        if (e.id == id) return &e;
    }
    return nullptr;
}

void EntityPalette::drawEntityIcon(const EntityDef& def, Rectangle dest, bool selected) const {
    // Background
    Color bg = selected ? Color{60, 120, 180, 255} : Color{35, 35, 50, 255};
    DrawRectangleRec(dest, bg);

    // Texture hoặc fallback
    if (!def.texturePath.empty()) {
        const auto& tex = EditorTextureCache::getInstance().getOrDefault(def.texturePath);
        if (tex.id != 0) {
            // Scale UV vào dest, giữ tỉ lệ
            float innerW = dest.width - 8;
            float innerH = dest.height - LABEL_HEIGHT - 8;
            
            float scaleX = innerW / std::abs(def.uv.width);
            float scaleY = innerH / std::abs(def.uv.height);
            float minScale = std::min(scaleX, scaleY);
            
            float newW = std::abs(def.uv.width) * minScale;
            float newH = std::abs(def.uv.height) * minScale;
            float offX = (innerW - newW) / 2.0f;
            float offY = (innerH - newH) / 2.0f;
            
            Rectangle imgDest = { dest.x + 4 + offX, dest.y + 4 + offY, newW, newH };
            DrawTexturePro(tex, def.uv, imgDest, {0, 0}, 0.0f, WHITE);
        } else {
            // Texture chưa load → fallback color
            DrawRectangleRec({dest.x + 4, dest.y + 4, dest.width - 8, dest.height - 20}, def.fallbackColor);
        }
    } else {
        // Không có texture → fallback color với chữ P
        DrawRectangleRec({dest.x + 4, dest.y + 4, dest.width - 8, dest.height - 20}, def.fallbackColor);
        DrawText("?", (int)(dest.x + dest.width / 2 - 5), (int)(dest.y + 10), 14, WHITE);
    }

    // Border
    float borderW = selected ? 2.5f : 1.0f;
    Color borderC = selected ? Color{100, 180, 255, 255} : Color{70, 70, 100, 180};
    DrawRectangleLinesEx(dest, borderW, borderC);

    // Label
    Color col = selected ? WHITE : Color{190, 190, 210, 200};
    int labelTw = MeasureText(def.displayName.c_str(), 20);
    int maxW = (int)dest.width;
    
    // Draw Name
    if (labelTw <= maxW) {
        DrawText(def.displayName.c_str(), 
                 (int)(dest.x + (dest.width - labelTw) / 2), 
                 (int)(dest.y + dest.height + 4), 20, col);
    } else {
        // Cắt bớt nếu tên quá dài
        std::string shortName = def.displayName.substr(0, 10) + "..";
        int sw = MeasureText(shortName.c_str(), 20);
        DrawText(shortName.c_str(), 
                 (int)(dest.x + (dest.width - sw) / 2), 
                 (int)(dest.y + dest.height + 4), 20, col);
    }
}

void EntityPalette::render(Rectangle panelRect) const {
    DrawRectangleRec(panelRect, Color{20, 20, 35, 240});
    DrawRectangleLinesEx(panelRect, 1.0f, Color{70, 70, 100, 255});

    BeginScissorMode((int)panelRect.x, (int)panelRect.y, (int)panelRect.width, (int)panelRect.height);

    float x = panelRect.x + ICON_PAD - scrollOffsetX_;
    float y = panelRect.y + ICON_PAD;

    for (const auto& def : entities_) {
        if (x + ICON_SIZE < panelRect.x || x > panelRect.x + panelRect.width) {
            x += ICON_SIZE + ICON_PAD;
            continue;
        }
        Rectangle iconRect = { x, y, ICON_SIZE, ICON_SIZE };
        drawEntityIcon(def, iconRect, def.id == selectedId_);
        x += ICON_SIZE + ICON_PAD;
    }
    
    EndScissorMode();
}

void EntityPalette::handleInput(Rectangle panelRect) {
    Vector2 mp = GetMousePosition();
    if (CheckCollisionPointRec(mp, panelRect)) {
        float wheel = GetMouseWheelMove();
        if (wheel != 0.0f) {
            scrollOffsetX_ -= wheel * 40.0f;
            float totalWidth = ICON_PAD + entities_.size() * (ICON_SIZE + ICON_PAD);
            float visibleWidth = panelRect.width;
            float maxScroll = totalWidth > visibleWidth ? (totalWidth - visibleWidth) : 0.0f;
            if (scrollOffsetX_ < 0.0f) scrollOffsetX_ = 0.0f;
            if (scrollOffsetX_ > maxScroll) scrollOffsetX_ = maxScroll;
        }
    }

    if (!IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) return;
    if (!CheckCollisionPointRec(mp, panelRect)) return;

    float x = panelRect.x + ICON_PAD - scrollOffsetX_;
    float y = panelRect.y + ICON_PAD;

    for (const auto& def : entities_) {
        Rectangle iconRect = { x, y, ICON_SIZE, ICON_SIZE };
        if (CheckCollisionPointRec(mp, iconRect)) {
            selectedId_ = def.id;
            return;
        }
        x += ICON_SIZE + ICON_PAD;
    }
}
