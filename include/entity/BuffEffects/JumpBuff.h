#pragma once
#include "IBuffEffect.h"
#include "Item/AtlasAnimation.h"
#include "Player.h"
#include <raylib.h>

class JumpBuff : public IBuffEffect {
private:
    AtlasAnimation auraAnim_;

public:
    JumpBuff() : auraAnim_("aura_jump", 12, 0.05f, true) {}

    std::string getName() const override { return "Jump"; }
    std::string getFrameName() const override { return "item_jump_drop"; }
    float getDuration() const override { return 8.0f; }
    
    // +50% Jump Power
    float getJumpMultiplier() const override { return 0.5f; }

    std::unique_ptr<IBuffEffect> clone() const override {
        return std::make_unique<JumpBuff>(*this);
    }
    
    void update(float dt, Player& player) override {
        auraAnim_.update(dt);
    }
    
    void render(const Player& player, float alpha) const override {
        if (!auraAnim_.isValid()) return;

        Rectangle src = auraAnim_.getCurrentSourceRect();
        const Texture2D& tex = auraAnim_.getTexture();
        if (tex.id == 0) return;

        Rectangle hitbox = player.getHitbox();
        float w = std::abs(src.width);
        float h = std::abs(src.height);

        Rectangle dest = {
            hitbox.x + hitbox.width / 2.0f - w / 2.0f,
            hitbox.y + hitbox.height - h,
            w, h
        };

        DrawTexturePro(tex, src, dest, {0,0}, 0.0f, WHITE);
    }
};
