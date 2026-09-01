#pragma once
#include "BaseItem.h"
#include "raylib.h"

// =============================================================================
// ShopAsset — Thực thể (Entity) đóng vai trò làm cửa hàng trên Map.
//
// Tuân thủ quy tắc OOP và 4 giai đoạn Game Loop.
// Không tự chứa UI, chỉ kích hoạt cờ (flag) để BaseLevelState mở ShopUI.
// =============================================================================
class ShopAsset : public BaseItem {
private:
    Texture2D shopTex_ = {0};
    bool requestOpenShop_ = false;
    bool isPlayerNear_ = false;

public:
    // Kích thước hitbox mặc định là 48x48 (3 block 16x16)
    ShopAsset(Vector2 worldPos);
    virtual ~ShopAsset();

    void update(float dt) override;
    void process(const std::vector<class Player*>& players) override;
    void render(float alpha) override;

    // Khi người chơi ấn Interact, hàm này    // Được gọi khi player va chạm
    void onInteract(Entity& other) override;
    // Được gọi khi player bấm Tương Tác
    void forceInteract(Entity& other) override;

    // Trả về cờ yêu cầu mở shop cho BaseLevelState
    bool wantsToOpenShop() const { return requestOpenShop_; }
    void resetOpenShop() { requestOpenShop_ = false; }
};
