#pragma once
#include "EditorBlockRegistry.h"
#include "raylib.h"
#include <string>
#include <vector>
#include <functional>

// =============================================================================
// BlockVariantPanel — Panel bên PHẢI của Bottom Panel.
// Hiển thị grid icon 48x48px các block thuộc category đang chọn.
// Mỗi icon: vẽ texture tile từ EditorTextureCache hoặc fallbackColor.
//
// Design: Dùng EditorTextureCache (Flyweight) thay vì giữ copy textures_.
// =============================================================================
class BlockVariantPanel {
public:
    static constexpr float ICON_SIZE    = 128.0f;
    static constexpr float ICON_PAD     = 15.0f;
    static constexpr float LABEL_HEIGHT = 28.0f;

    // Khi block được chọn → callback trả về blockId mới
    using OnBlockSelected = std::function<void(const std::string&)>;

    explicit BlockVariantPanel(OnBlockSelected callback);

    // Gọi khi category thay đổi (Observer từ CategoryPanel)
    void onCategoryChanged(const std::string& category);

    void render(Rectangle panelRect) const;
    void handleInput(Rectangle panelRect);

    const std::string& getSelectedBlockId() const { return selectedBlockId_; }

private:
    std::vector<std::string>  blockIds_;
    std::string               selectedBlockId_;
    OnBlockSelected           callback_;
    int scrollOffsetX_ = 0;

    void drawBlockIcon(const EditorBlockDef& def, Rectangle dest, bool isSelected) const;
};
