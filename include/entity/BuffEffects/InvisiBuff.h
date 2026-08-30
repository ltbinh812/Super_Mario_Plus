#pragma once
#include "IBuffEffect.h"
#include "Item/AtlasAnimation.h"
#include "Player.h"
#include <raylib.h>

class InvisiBuff : public IBuffEffect {
private:
    AtlasAnimation auraAnim_;

public:
    InvisiBuff() : auraAnim_("aura_invisible", 12, 0.05f, true) {}

    std::string getName() const override { return "Invisibility"; }
    std::string getFrameName() const override { return "item_invisibility_drop"; }
    float getDuration() const override { return 6.0f; }
    
    bool isInvincible() const override { return true; }
    bool isInvisible() const override { return true; }

    std::unique_ptr<IBuffEffect> clone() const override {
        return std::make_unique<InvisiBuff>(*this);
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
