#include "CategoryPanel.h"
#include <iostream>

static const char* categoryIcon(const std::string& cat) {
    if (cat == "Solid")         return "# Solid";
    if (cat == "OneWay")        return "- OneWay";
    if (cat == "Ladder")        return "^ Ladder";
    if (cat == "Lotus")         return "~ Lotus";
    if (cat == "Special Block") return "* Special Block";
    if (cat == "Entities")      return "E Entities";
    return cat.c_str();
}

CategoryPanel::CategoryPanel(OnCategoryChanged callback)
    : callback_(std::move(callback)) {}

void CategoryPanel::init() {
    categories_ = EditorBlockRegistry::getInstance().getOrderedCategories();
    categories_.push_back("Entities");  // Tab entity luôn ở cuối
    if (!categories_.empty()) {
        selected_ = categories_.front();
        if (callback_) callback_(selected_);
    }
}

void CategoryPanel::render(Rectangle panelRect) const {
    // Background
    DrawRectangleRec(panelRect, Color{20, 20, 30, 240});
    DrawRectangleLinesEx(panelRect, 1.0f, Color{80, 80, 100, 255});

    float totalHeight = categories_.size() * ITEM_HEIGHT + 8.0f;
    float visibleHeight = panelRect.height;
    bool showScrollbar = totalHeight > visibleHeight;
    float maxScroll = showScrollbar ? (totalHeight - visibleHeight) : 0.0f;

    // Tính toán chiều rộng khả dụng của item
    float itemWidth = showScrollbar ? (panelRect.width - 14) : (panelRect.width - 4);

    BeginScissorMode((int)panelRect.x, (int)panelRect.y, (int)panelRect.width, (int)panelRect.height);

    float y = panelRect.y + 4.0f - scrollOffset_;
    for (const auto& cat : categories_) {
        // Chỉ vẽ nếu lọt vào vùng hiển thị
        if (y + ITEM_HEIGHT >= panelRect.y && y <= panelRect.y + panelRect.height) {
            Rectangle itemRect = { panelRect.x + 2, y, itemWidth, ITEM_HEIGHT };
            bool isSelected = (cat == selected_);

            Color bg = isSelected ? Color{60, 100, 180, 255} : Color{30, 30, 45, 0};
            DrawRectangleRec(itemRect, bg);
            if (isSelected) DrawRectangleLinesEx(itemRect, 1.0f, Color{100, 160, 255, 200});

            DrawText(categoryIcon(cat),
                     (int)(panelRect.x + 12), (int)(y + 18),
                     24, isSelected ? WHITE : Color{180, 180, 200, 255});
        }
        y += ITEM_HEIGHT;
    }

    EndScissorMode();

    // Vẽ thanh cuộn nếu cần
    if (showScrollbar) {
        Rectangle track = { panelRect.x + panelRect.width - 10, panelRect.y, 10, panelRect.height };
        DrawRectangleRec(track, Color{30, 30, 45, 255});

        float thumbHeight = (visibleHeight / totalHeight) * visibleHeight;
        if (thumbHeight < 20.0f) thumbHeight = 20.0f;
        float thumbY = panelRect.y + (scrollOffset_ / maxScroll) * (visibleHeight - thumbHeight);
        Rectangle thumb = { track.x + 2, thumbY, 6, thumbHeight };
        DrawRectangleRec(thumb, Color{100, 160, 255, 180});
    }
}

void CategoryPanel::handleInput(Rectangle panelRect) {
    Vector2 mp = GetMousePosition();
    if (!CheckCollisionPointRec(mp, panelRect)) return;

    // Xử lý sự kiện lăn chuột để cuộn danh sách
    float totalHeight = categories_.size() * ITEM_HEIGHT + 8.0f;
    float visibleHeight = panelRect.height;
    float maxScroll = (totalHeight > visibleHeight) ? (totalHeight - visibleHeight) : 0.0f;

    float wheel = GetMouseWheelMove();
    if (wheel != 0.0f) {
        scrollOffset_ -= (int)(wheel * ITEM_HEIGHT * 2.0f);
    }
    
    // Giới hạn cuộn
    if (scrollOffset_ < 0) scrollOffset_ = 0;
    if (scrollOffset_ > maxScroll) scrollOffset_ = maxScroll;

    if (!IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) return;

    // Xử lý click chọn item
    float y = panelRect.y + 4.0f - scrollOffset_;
    for (const auto& cat : categories_) {
        // Chỉ nhận click nếu item đang hiển thị trong vùng của panelRect
        if (y + ITEM_HEIGHT >= panelRect.y && y <= panelRect.y + panelRect.height) {
            Rectangle itemRect = { panelRect.x + 2, y, panelRect.width - 4, ITEM_HEIGHT };
            if (CheckCollisionPointRec(mp, itemRect)) {
                if (selected_ != cat) {
                    selected_ = cat;
                    if (callback_) callback_(selected_);
                }
                return;
            }
        }
        y += ITEM_HEIGHT;
    }
}
