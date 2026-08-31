#pragma once
#include "EditorBlockRegistry.h"
#include "raylib.h"
#include <string>
#include <vector>
#include <functional>

// Panel bên TRÁI của Bottom Panel.
// Hiển thị danh sách category (Solid, OneWay, Ladder, Water, Hazard, Lava, Cloud, Die, Decoration, Entities)
// Khi click vào category → gọi callback onCategoryChanged.
//
// Observer pattern đơn giản: dùng std::function callback.
class CategoryPanel {
public:
    static constexpr float WIDTH       = 300.0f;
    static constexpr float ITEM_HEIGHT = 60.0f;

    // callback nhận category name (string) khi user click
    using OnCategoryChanged = std::function<void(const std::string&)>;

    explicit CategoryPanel(OnCategoryChanged callback);

    void init();   // Load categories từ EditorBlockRegistry + thêm "Entities"

    void render(Rectangle panelRect) const;
    void handleInput(Rectangle panelRect);

    const std::string& getSelectedCategory() const { return selected_; }

private:
    std::vector<std::string> categories_;
    std::string              selected_;
    OnCategoryChanged        callback_;
    int                      scrollOffset_ = 0;  // scroll nếu nhiều category
};
