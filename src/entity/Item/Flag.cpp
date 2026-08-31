#include "Flag.h"
#include <iostream>

static const float BLOCK_SIZE = 32.0f;

Flag::Flag(Vector2 worldPos, float scale)
    : BaseItem(worldPos, BLOCK_SIZE * 3.0f, BLOCK_SIZE * 3.0f) 
{
    animations_[ItemState::Active] = AtlasAnimation("flag_active_anim", 5, 0.1f, true);
    animations_[ItemState::Idle] = AtlasAnimation("flag_idle_anim", 1, 0.1f, false);
    setAnimation(ItemState::Idle);
}

void Flag::render(float alpha) {
    if (!currentAnim_ || !currentAnim_->isValid()) return;

    const Texture2D& tex = currentAnim_->getTexture();
    if (tex.id == 0) return;

    Rectangle src = currentAnim_->getCurrentSourceRect();
    
    // Draw the 64x64 frame scaled up to match the 96x96 physical hitbox
    float drawW = 96.0f;
    float drawH = 96.0f;
    
    Rectangle dest = {
        worldStats.position.x - (drawW - hitW_) / 2.0f,
        worldStats.position.y - drawH,
        drawW,
        drawH
    };
    
    DrawTexturePro(tex, src, dest, { 0, 0 }, 0.0f, WHITE);
}

void Flag::onInteract(Entity& other) {
    if (itemState_ == ItemState::Active) return;
    itemState_ = ItemState::Active;
    setAnimation(ItemState::Active);
    std::cout << "[Flag] Activated!\n";
}
