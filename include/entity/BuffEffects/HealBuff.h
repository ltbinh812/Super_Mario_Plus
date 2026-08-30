#pragma once
#include "IBuffEffect.h"
#include "Player.h"
#include "Item/AtlasAnimation.h"
#include <raylib.h>
#include <iostream>

class HealBuff : public IBuffEffect {
private:
    AtlasAnimation auraAnim_;
    float tickTimer = 0.0f;
    float duration = 5.0f;

public:
    HealBuff() : auraAnim_("aura_heal", 12, 0.05f, true) {}

    std::string getName() const override { return "Heal"; }
    std::string getFrameName() const override { return "item_heal_drop"; }
    int getFrameCount() const override { return 30; }
    float getFrameTime() const override { return 0.05f; }
    float getDuration() const override { return duration; }
    
    std::unique_ptr<IBuffEffect> clone() const override {
        return std::make_unique<HealBuff>(*this);
    }

    void onApply(Player& player) override {
        // No instant heal anymore
    }

    void update(float dt, Player& player) override {
        auraAnim_.update(dt);
        tickTimer += dt;
        if (tickTimer >= 0.5f) { // Heal 5 HP every 0.5 seconds (50 HP total over 5s)
            auto& stats = player.getRuntimeStatsMutable();
            const auto& baseStats = player.getBaseStats();
            stats.health += 5; 
            if (stats.health > baseStats.maxHealth) {
                stats.health = baseStats.maxHealth;
            }
            tickTimer -= 0.5f;
            
            // Add visual floating text for heal
            player.addFloatingText("+5", GREEN, {0, -20.0f}, 1.0f);
        }
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
