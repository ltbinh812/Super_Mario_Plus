#include "ChestNormal.h"
#include "Entity.h"
#include <cstdlib>
#include <iostream>
#include <cmath>
#include "CommandQueue.h"
#include "ItemUsageFactory.h"
#include "infrastructure/AssetManager.h"

static const float HITBOX_W = 32.0f;
static const float HITBOX_H = 32.0f;

ChestNormal::ChestNormal(Vector2 worldPos, float scale)
    : BaseItem(worldPos, HITBOX_W, HITBOX_H)
{
    // 4 types of chests: 1 to 4
    chestType_ = (rand() % 4) + 1;
    
    std::string idleFrame = "normal_chest_" + std::to_string(chestType_) + "_idle";
    std::string activeFrame = "normal_chest_" + std::to_string(chestType_) + "_active";

    animations_[ItemState::Idle] = AtlasAnimation(idleFrame, 5, 0.1f, true);
    animations_[ItemState::Active] = AtlasAnimation(activeFrame, 5, 0.1f, false);
    
    setAnimation(ItemState::Idle);
}

void ChestNormal::render(float alpha) {
    if (!currentAnim_ || !currentAnim_->isValid()) return;

    const Texture2D& tex = currentAnim_->getTexture();
    if (tex.id == 0) return;

    Rectangle src = currentAnim_->getCurrentSourceRect();
    
    // Draw using the chest's frame size (96x64), centered on the 32x32 hitbox
    float drawW = 96.0f;
    float drawH = 64.0f;
    
    Rectangle dest = {
        worldStats.position.x - (drawW - hitW_) / 2.0f,
        worldStats.position.y - drawH + getRenderOffsetY(),
        drawW,
        drawH
    };
    
    DrawTexturePro(tex, src, dest, { 0, 0 }, 0.0f, WHITE);
}

void ChestNormal::onInteract(Entity& other) {
    if (itemState_ == ItemState::Active) return;
    itemState_ = ItemState::Active;
    setAnimation(ItemState::Active);

    PlaySound(AssetManager::getInstance().getSound("chest_open_sound"));

    if (commandQueue) {
        SpawnCommand cmd;
        cmd.category = SpawnCategory::Item;
        
        // Bốc đều trong TOÀN BỘ danh sách vật phẩm dùng được: 8 buff (Speed,
        // Strength, Shield, Jump, Invisibility, GoldMagnet, TimeStop, Heal)
        // cộng Poison và Boom.
        //
        // Trước đây rương chỉ có ba khả năng — Coin, Boom, hoặc "Buff" chung
        // chung — mà nhánh "Buff" lại đẩy sang bảng quay của Random buff, và
        // bảng đó bỏ sót Strength, Invisibility, TimeStop. Kết quả: ba buff ấy
        // KHÔNG BAO GIỜ ra khỏi rương. Nay lấy thẳng danh sách gốc nên không
        // thể sót.
        //
        // Coin bị bỏ khỏi bể này vì tiền giờ đã rơi từ quái (xem EnemyDieState);
        // rương để dành cho vật phẩm.
        const auto& pool = ItemUsageFactory::allUsableItems();
        cmd.itemIdentifier = pool[rand() % pool.size()];

        // Gửi vị trí CỦA CHÍNH RƯƠNG, không phải chỗ đã nhấc lên sẵn.
        // BaseLevelState::findFreeItemSpawn() sẽ tự nhấc lên nếu phía trên
        // trống, còn vướng đá thì thả ngay tại đây — chỗ này chắc chắn đi tới
        // được, vì người chơi vừa đứng đó mở rương.
        cmd.position = worldStats.position;
        commandQueue->push(cmd);
    }

    std::cout << "[ChestNormal] Opened! Spawning random item.\n";
}

float ChestNormal::getRenderOffsetY() const {
    return 0.0f;
}
