#include "BlockVariantPanel.h"
#include "EditorBlockRegistry.h"
#include "EditorTextureCache.h"
#include <algorithm>

BlockVariantPanel::BlockVariantPanel(OnBlockSelected callback)
    : callback_(std::move(callback)) {}

void BlockVariantPanel::onCategoryChanged(const std::string& category) {
    blockIds_ = EditorBlockRegistry::getInstance().getAllInCategory(category);
    scrollOffsetX_ = 0;

    // Auto-select đầu tiên nếu current selection không còn trong category
    if (!blockIds_.empty()) {
        bool stillValid = std::find(blockIds_.begin(), blockIds_.end(), selectedBlockId_) != blockIds_.end();
        if (!stillValid || selectedBlockId_.empty()) {
            selectedBlockId_ = blockIds_.front();
            if (callback_) callback_(selectedBlockId_);
        }
    }
}

void BlockVariantPanel::drawBlockIcon(const EditorBlockDef& def, Rectangle dest, bool isSelected) const {
    // Background
    DrawRectangleRec(dest, Color{35, 35, 50, 255});

    // Texture từ EditorTextureCache (Flyweight — không copy, chỉ reference)
    if (!def.tilesetPath.empty()) {
        const Texture2D& tex = EditorTextureCache::getInstance().getOrDefault(def.tilesetPath);
        if (tex.id != 0) {
            float scaleX = dest.width / std::abs(def.uv.width);
            float scaleY = dest.height / std::abs(def.uv.height);
            float minScale = std::min(scaleX, scaleY);
            
            float newW = std::abs(def.uv.width) * minScale;
            float newH = std::abs(def.uv.height) * minScale;
            float offX = (dest.width - newW) / 2.0f;
            float offY = (dest.height - newH) / 2.0f;
            
            Rectangle imgDest = { dest.x + offX, dest.y + offY, newW, newH };
            
            DrawTexturePro(tex, def.uv, imgDest, {0, 0}, 0.0f, WHITE);
        } else {
            DrawRectangleRec(dest, def.fallbackColor);
        }
    } else {
        DrawRectangleRec(dest, def.fallbackColor);
    }

    // Selection border
    float borderW = isSelected ? 3.0f : 1.0f;
    Color borderC = isSelected ? Color{100, 200, 255, 255} : Color{80, 80, 100, 180};
    DrawRectangleLinesEx(dest, borderW, borderC);

    // Label dưới icon
    Rectangle labelRect = { dest.x, dest.y + dest.height, dest.width, LABEL_HEIGHT };
    Color col = isSelected ? WHITE : Color{190, 190, 210, 200};
    int labelTw = MeasureText(def.displayName.c_str(), 20);
    int maxW = (int)labelRect.width;
    
    // Draw Name
    if (labelTw <= maxW) {
        DrawText(def.displayName.c_str(), 
                 (int)(labelRect.x + (labelRect.width - labelTw) / 2), 
                 (int)(labelRect.y + 4), 20, col);
    } else {
        // Cắt bớt nếu tên quá dài
        std::string shortName = def.displayName.substr(0, 10) + "..";
        int sw = MeasureText(shortName.c_str(), 20);
        DrawText(shortName.c_str(), 
                 (int)(labelRect.x + (labelRect.width - sw) / 2), 
                 (int)(labelRect.y + 4), 20, col);
    }
}

void BlockVariantPanel::render(Rectangle panelRect) const {
    DrawRectangleRec(panelRect, Color{22, 22, 38, 240});
    DrawRectangleLinesEx(panelRect, 1.0f, Color{70, 70, 100, 255});

    auto& reg = EditorBlockRegistry::getInstance();
    float startX = panelRect.x + ICON_PAD - (float)scrollOffsetX_;
    float y      = panelRect.y + ICON_PAD;

    // Bật chế độ cắt hình để đảm bảo các khối trượt ra ngoài lề trái/phải sẽ bị ẩn
    BeginScissorMode((int)panelRect.x, (int)panelRect.y, (int)panelRect.width, (int)panelRect.height);

    for (const auto& id : blockIds_) {
        if (!reg.has(id)) continue;
        const auto& def = reg.get(id);

        Rectangle iconRect = { startX, y, ICON_SIZE, ICON_SIZE };

        drawBlockIcon(def, iconRect, id == selectedBlockId_);
        startX += ICON_SIZE + ICON_PAD;
    }

    EndScissorMode();
}

void BlockVariantPanel::handleInput(Rectangle panelRect) {
    // Scroll ngang bằng scroll wheel khi hover trên panel
    Vector2 mp = GetMousePosition();
    
    // Tính giới hạn cuộn
    float totalWidth = ICON_PAD + blockIds_.size() * (ICON_SIZE + ICON_PAD);
    int maxScroll = 0;
    if (totalWidth > panelRect.width) {
        maxScroll = (int)(totalWidth - panelRect.width);
    }

    if (CheckCollisionPointRec(mp, panelRect)) {
        float wheel = GetMouseWheelMove();
        if (wheel != 0.0f) {
            scrollOffsetX_ -= (int)(wheel * 30.0f);
        }
    }

    // Giới hạn cuộn
    if (scrollOffsetX_ < 0) scrollOffsetX_ = 0;
    if (scrollOffsetX_ > maxScroll) scrollOffsetX_ = maxScroll;


    if (!IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) return;
    if (!CheckCollisionPointRec(mp, panelRect)) return;

    auto& reg    = EditorBlockRegistry::getInstance();
    float startX = panelRect.x + ICON_PAD - (float)scrollOffsetX_;
    float y      = panelRect.y + ICON_PAD;

    for (const auto& id : blockIds_) {
        Rectangle iconRect = { startX, y, ICON_SIZE, ICON_SIZE };
        if (CheckCollisionPointRec(mp, iconRect)) {
            if (selectedBlockId_ != id) {
                selectedBlockId_ = id;
                if (callback_) callback_(id);
            }
            return;
        }
        startX += ICON_SIZE + ICON_PAD;
    }
}
