#include "Effects.h"
#include "Entity.h"
#include "ItemAtlasRegistry.h"
#include <cmath>
#include <string>

bool PoisonEffect::update(Entity& entity, float dt) {
    tickTimer += dt;
    float tickLimit = inPoison ? 0.2f : 1.0f;
    
    if (tickTimer >= tickLimit) {
        entity.takeDamage(1, false); 
        tickTimer = 0.0f;
    }
    
    if (!inPoison) {
        duration -= dt;
    }

    animTimer1 += dt;
    animTimer2 += dt;
    
    currentFrame1 = static_cast<int>(animTimer1 / 0.1f) % 12;
    currentFrame2 = static_cast<int>(animTimer2 / 0.1f) % 8;

    return duration <= 0.0f;
}

void PoisonEffect::refresh() {
    inPoison = true;
    duration = 5.0f;
    // Do not reset animTimers to allow smooth looping while inside poison
}

void PoisonEffect::render(const Entity& entity, float alpha) {
    auto& reg = ItemAtlasRegistry::getInstance();
    
    Rectangle hitbox = entity.getHitbox();
    float scale1 = 1.0f;
    float scale2 = 1.0f;

    std::string frameName1 = "poison_effect1_" + std::to_string(currentFrame1);
    Rectangle src1 = reg.getFrame(frameName1);
    const Texture2D& tex1 = reg.getTexture(frameName1);
    if (tex1.id != 0) {
        float w1 = src1.width * scale1;
        float h1 = src1.height * scale1;
        Rectangle dest1 = {
            hitbox.x + hitbox.width / 2.0f - w1 / 2.0f,
            hitbox.y + hitbox.height - h1 + 15.0f, // Thấp xuống một xíu
            w1, h1
        };
        DrawTexturePro(tex1, src1, dest1, {0,0}, 0.0f, WHITE);
    }

    std::string frameName2 = "poison_effect2_" + std::to_string(currentFrame2);
    Rectangle src2 = reg.getFrame(frameName2);
    const Texture2D& tex2 = reg.getTexture(frameName2);
    if (tex2.id != 0) {
        float w2 = src2.width * scale2;
        float h2 = src2.height * scale2;
        Rectangle dest2 = {
            hitbox.x + hitbox.width / 2.0f - w2 / 2.0f,
            hitbox.y + hitbox.height - h2, // Sát mặt đất (đáy hitbox)
            w2, h2
        };
        DrawTexturePro(tex2, src2, dest2, {0,0}, 0.0f, WHITE);
    }
}

LavaEffect::LavaEffect() : anim_("fire_effect", 8, 0.1f, true) {}

bool LavaEffect::update(Entity& entity, float dt) {
    tickTimer += dt;
    float tickLimit = inLava ? 0.2f : 0.5f;
    
    if (tickTimer >= tickLimit) {
        entity.takeDamage(1, false); // 1 damage per tick, no interrupt
        tickTimer = 0.0f;
    }
    
    if (!inLava) {
        duration -= dt;
    }

    anim_.update(dt);

    return duration <= 0.0f; // Remove when duration ends (only counts down when outside)
}

void LavaEffect::refresh() {
    inLava = true;
    duration = 5.0f; // Reset outside duration
    // Do not reset animTimer so the fire animation continues smoothly
}

void LavaEffect::render(const Entity& entity, float alpha) {
    if (!anim_.isValid()) return;
    
    Rectangle src = anim_.getCurrentSourceRect();
    const Texture2D& tex = anim_.getTexture();
    if (tex.id == 0) return;

    Rectangle hitbox = entity.getHitbox();
    float scale = 1.0f;
    float w = src.width * scale;
    float h = src.height * scale;

    Rectangle dest = {
        hitbox.x + hitbox.width / 2.0f - w / 2.0f,
        hitbox.y + hitbox.height - h + 15.0f, // slightly below bottom
        w, h
    };

    DrawTexturePro(tex, src, dest, {0,0}, 0.0f, Color{ 255, 255, 255, 200 }); // Slightly transparent
}
