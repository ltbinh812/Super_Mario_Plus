#pragma once
#include "BaseItem.h"
#include "GifAnimation.h"
#include <raylib.h>

// =============================================================================
// EndgameAsset — Cổng kết thúc màn chơi (entity "Endgame" trong file LDtk).
//
// Người chơi chạm vào -> isReached_ = true -> BaseLevelState chuyển sang
// EndgameState.
//
// Phần hình ảnh do GifAnimation lo: nó nạp gate.gif và tự đổi khung hình, nên
// class này không phải tự quản lý Image/Texture/bộ đếm frame nữa.
// =============================================================================
class EndgameAsset : public BaseItem {
private:
    GifAnimation gateAnim_;

    // Kích thước vẽ ra màn hình (pixel game), độc lập với kích thước file GIF.
    static constexpr float kRenderWidth  = 64.0f;
    static constexpr float kRenderHeight = 128.0f;

public:
    bool isReached_;

    EndgameAsset(Vector2 worldPos);
    ~EndgameAsset() override = default;

    void update(float dt) override;
    void process(const std::vector<class Player*>& players) override;
    void render(float alpha) override;
    void onInteract(Entity& other) override;
    void forceInteract(Entity& other) override;
};
