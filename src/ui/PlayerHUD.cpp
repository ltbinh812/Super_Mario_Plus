#include "PlayerHUD.h"
#include "raylib.h"
#include "ItemAtlasRegistry.h"
#include "AssetManager.h"
#include <string>
#include <algorithm>

static std::string getFrameName(const std::string& itemIdentifier) {
    if (itemIdentifier == "Boom") return "bomb.png";
    if (itemIdentifier == "Speed") return "item_speed_fix01 (Custom).png";
    if (itemIdentifier == "Strength") return "item_strength_fix01 (Custom).png";
    if (itemIdentifier == "Shield") return "item_shield_fix01 (Custom).png";
    if (itemIdentifier == "Jump") return "item_jump_fix01 (Custom).png";
    if (itemIdentifier == "Invisibility") return "item_invisibility_fix01 (Custom).png";
    if (itemIdentifier == "GoldMagnet") return "item_gold_magnet_fix01 (Custom).png";
    if (itemIdentifier == "TimeStop") return "item_time_stop_fix01 (Custom).png";
    return "";
}

void PlayerHUD::render(const Player* p1, const PartyInventory* inventory) {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    static bool initHUD = false;
    static Texture2D avatarTex = {0};
    static Texture2D frameTex = {0};
    static Font customFont = {0};
    
    if (!initHUD) {
        AssetManager::getInstance().loadTexture("hud_item_empty", "assets/maps/item/item_empty.png");
        AssetManager::getInstance().loadTexture("partyhud", "assets/maps/item/partyhud.png");
        AssetManager::getInstance().loadTexture("boom_item", "assets/maps/item/boom_item.png");
        
        // Mask avatar as circle
        Image img = LoadImage("assets/Goku_animation/avatar.png");
        ImageResize(&img, 168, 168);
        
        Image mask = GenImageColor(168, 168, BLANK);
        ImageDrawCircle(&mask, 84, 84, 84, WHITE);
        
        // Ensure image format has alpha channel
        ImageFormat(&img, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
        ImageAlphaMask(&img, mask);
        
        avatarTex = LoadTextureFromImage(img);
        
        UnloadImage(img);
        UnloadImage(mask);
        
        // Load portrait frame
        frameTex = LoadTexture("assets/maps/item/dfgui_portraitframe_player.png");

        // Load custom font
        customFont = LoadFont("assets/config/kenney-pixel-hu.otf");

        initHUD = true;
    }

    Texture2D itemEmptyTex = AssetManager::getInstance().getTexture("hud_item_empty");
    Texture2D partyhudTex = AssetManager::getInstance().getTexture("partyhud");
    Texture2D boomItemTex = AssetManager::getInstance().getTexture("boom_item");

    auto drawPlayerHUD = [&](const Player* p, bool isLeft) {
        if (!p) return;

        int hp = p->getRuntimeStats().health;
        int maxHp = p->getBaseStats().maxHealth;
        int mp = p->getRuntimeStats().mana;
        int maxMp = p->getBaseStats().maxMana;
        int breath = p->getRuntimeStats().breath;
        int maxBreath = p->getBaseStats().maxBreath;
        std::string name = p->getBaseStats().name;
        std::string item = p->getRuntimeStats().storedItemSlot;

        // Base coordinates
        float avatarY = screenHeight - 250;
        float startX = isLeft ? 40 : (screenWidth - 737);
        
        auto drawBarsAndItem = [&](float barX, bool isLeftHUD) {
            float scaleX = 3.0f;
            float scaleY = 3.0f; // 50% taller than 2.0f
            float barY = avatarY + 7.0f; // Vertically center the 218px block with the 232px avatar
            
            // Draw HP (single_bar)
            Rectangle hpBgSrc = {59, 1, 145, 20};
            DrawTexturePro(partyhudTex, hpBgSrc, {barX, barY, 145 * scaleX, 20 * scaleY}, {0,0}, 0.0f, WHITE);
            
            float hpPercent = (float)std::max(0, hp) / maxHp;
            Rectangle hpFillSrc = {59, 41, 125.0f * hpPercent, 15};
            DrawTexturePro(partyhudTex, hpFillSrc, {barX + 10 * scaleX, barY + 3 * scaleY, 125 * scaleX * hpPercent, 15 * scaleY}, {0,0}, 0.0f, WHITE);
            
            std::string hpText = std::to_string(std::max(0, hp)) + "/" + std::to_string(maxHp);
            int hpTextW = MeasureTextEx(customFont, hpText.c_str(), 24, 1.0f).x;
            DrawTextEx(customFont, hpText.c_str(), {barX + (145 * scaleX - hpTextW) / 2.0f, barY + 18}, 24, 1.0f, WHITE);
            
            // Draw MP & Breath (double_bar)
            float mpBarY = barY + 20 * scaleY + 5; // 5px gap
            Rectangle doubleBgSrc = {59, 23, 145, 16};
            DrawTexturePro(partyhudTex, doubleBgSrc, {barX, mpBarY, 145 * scaleX, 16 * scaleY}, {0,0}, 0.0f, WHITE);
            
            float mpPercent = (float)std::max(0, mp) / maxMp;
            Rectangle mpFillSrc = {132, 72, 59.0f * mpPercent, 12};
            DrawTexturePro(partyhudTex, mpFillSrc, {barX + 10 * scaleX, mpBarY + 2 * scaleY, 59 * scaleX * mpPercent, 12 * scaleY}, {0,0}, 0.0f, WHITE);
            
            std::string mpText = std::to_string(std::max(0, mp)) + "/" + std::to_string(maxMp);
            int mpTextW = MeasureTextEx(customFont, mpText.c_str(), 20, 1.0f).x;
            DrawTextEx(customFont, mpText.c_str(), {barX + 10 * scaleX + (59 * scaleX - mpTextW) / 2.0f, mpBarY + 14}, 20, 1.0f, WHITE);
            
            float breathPercent = (float)std::max(0, breath) / maxBreath;
            Rectangle breathFillSrc = {132, 58, 59.0f * breathPercent, 12};
            DrawTexturePro(partyhudTex, breathFillSrc, {barX + 76 * scaleX, mpBarY + 2 * scaleY, 59 * scaleX * breathPercent, 12 * scaleY}, {0,0}, 0.0f, WHITE);
            
            std::string breathText = std::to_string(std::max(0, breath)) + "/" + std::to_string(maxBreath);
            int breathTextW = MeasureTextEx(customFont, breathText.c_str(), 20, 1.0f).x;
            DrawTextEx(customFont, breathText.c_str(), {barX + 76 * scaleX + (59 * scaleX - breathTextW) / 2.0f, mpBarY + 14}, 20, 1.0f, WHITE);

            // Draw Item Slot centered below the double_bar, aligned to left or right edge
            int itemSize = 100;
            float itemX = isLeftHUD ? barX : (barX + 145 * scaleX - itemSize);
            float itemY = mpBarY + 16 * scaleY + 5; // 5px gap below MP bar
            
            Rectangle itemDest = {itemX, itemY, (float)itemSize, (float)itemSize};
            
            if (!item.empty()) {
                std::string frameName = getFrameName(item);
                if (!frameName.empty()) {
                    Rectangle src = ItemAtlasRegistry::getInstance().getFrame(frameName);
                    Texture2D tex = ItemAtlasRegistry::getInstance().getTexture(frameName);
                    
                    if (item == "Boom") {
                        DrawTexturePro(boomItemTex, {0, 0, (float)boomItemTex.width, (float)boomItemTex.height}, itemDest, {0,0}, 0.0f, WHITE);
                    } else {
                        // Buff sprites already have their own frame, so replace the empty frame entirely
                        DrawTexturePro(tex, src, itemDest, {0,0}, 0.0f, WHITE);
                    }
                    
                    // Draw item name text next to the slot
                    float fontSize = 40.0f;
                    int textW = MeasureTextEx(customFont, item.c_str(), fontSize, 1.0f).x;
                    float textX = isLeftHUD ? (itemX + itemSize + 15) : (itemX - textW - 15);
                    float textY = itemY + (itemSize - fontSize) / 2.0f;
                    DrawTextEx(customFont, item.c_str(), {textX, textY}, fontSize, 1.0f, GOLD);
                } else {
                    DrawTexturePro(itemEmptyTex, {0, 0, (float)itemEmptyTex.width, (float)itemEmptyTex.height}, itemDest, {0,0}, 0.0f, WHITE);
                }
            } else {
                DrawTexturePro(itemEmptyTex, {0, 0, (float)itemEmptyTex.width, (float)itemEmptyTex.height}, itemDest, {0,0}, 0.0f, WHITE);
            }
        };

        if (isLeft) {
            // P1 layout: Avatar -> Bars
            DrawTextureEx(frameTex, {startX, avatarY}, 0.0f, 2.0f, WHITE);
            DrawTexture(avatarTex, startX + 32, avatarY + 32, WHITE);
            
            float p1TextSize = 40.0f;
            DrawTextEx(customFont, "Player 1", {startX, avatarY - 105}, p1TextSize, 1.0f, WHITE);
            
            float nameSize = 56.0f;
            float nameW = MeasureTextEx(customFont, name.c_str(), nameSize, 1.0f).x;
            DrawTextEx(customFont, name.c_str(), {startX + 116.0f - nameW / 2.0f, avatarY - 65}, nameSize, 1.0f, BLUE);

            // Draw active buffs
            const auto& buffs = p->getBuffManager().getActiveBuffs();
            float buffY = avatarY - 180.0f;
            for (const auto& buff : buffs) {
                if (buff.remainingTime <= 0.0f) continue;
                std::string bName = buff.effect->getName();
                std::string timeStr = TextFormat("%.1f s", buff.remainingTime);
                std::string frameName = getFrameName(bName);
                if (!frameName.empty()) {
                    Rectangle src = ItemAtlasRegistry::getInstance().getFrame(frameName);
                    Texture2D tex = ItemAtlasRegistry::getInstance().getTexture(frameName);
                    float iconSize = 64.0f;
                    float textSize = 50.0f;
                    DrawTexturePro(tex, src, {startX, buffY, iconSize, iconSize}, {0,0}, 0.0f, WHITE);
                    DrawTextEx(customFont, timeStr.c_str(), {startX + iconSize + 10, buffY + (iconSize - textSize) / 2.0f}, textSize, 1.0f, WHITE);
                    buffY -= (iconSize + 10);
                }
            }

            int barX = startX + 260; // Spacing after 232px avatar frame
            drawBarsAndItem((float)barX, isLeft);
        } else {
            // P2 layout: Bars -> Avatar
            int barX = startX;
            drawBarsAndItem((float)barX, isLeft);

            int frameX = barX + 465; // Bars width (145*3=435) + 30px spacing
            DrawTextureEx(frameTex, {(float)frameX, avatarY}, 0.0f, 2.0f, WHITE);
            DrawTexture(avatarTex, frameX + 32, avatarY + 32, WHITE);
            
            float p2TextSize = 40.0f;
            float p2W = MeasureTextEx(customFont, "Player 2", p2TextSize, 1.0f).x;
            DrawTextEx(customFont, "Player 2", {(float)frameX + 232.0f - p2W, avatarY - 105}, p2TextSize, 1.0f, WHITE);
            
            float nameSize = 56.0f;
            float nameW = MeasureTextEx(customFont, name.c_str(), nameSize, 1.0f).x;
            DrawTextEx(customFont, name.c_str(), {(float)frameX + 116.0f - nameW / 2.0f, avatarY - 65}, nameSize, 1.0f, GREEN);

            // Draw active buffs
            const auto& buffs = p->getBuffManager().getActiveBuffs();
            float buffY = avatarY - 180.0f;
            for (const auto& buff : buffs) {
                if (buff.remainingTime <= 0.0f) continue;
                std::string bName = buff.effect->getName();
                std::string timeStr = TextFormat("%.1f s", buff.remainingTime);
                std::string frameName = getFrameName(bName);
                if (!frameName.empty()) {
                    Rectangle src = ItemAtlasRegistry::getInstance().getFrame(frameName);
                    Texture2D tex = ItemAtlasRegistry::getInstance().getTexture(frameName);
                    float iconSize = 64.0f;
                    float textSize = 50.0f;
                    int textW = MeasureTextEx(customFont, timeStr.c_str(), textSize, 1.0f).x;
                    float rightEdge = frameX + 232.0f;
                    DrawTextEx(customFont, timeStr.c_str(), {rightEdge - textW, buffY + (iconSize - textSize) / 2.0f}, textSize, 1.0f, WHITE);
                    DrawTexturePro(tex, src, {rightEdge - textW - 10 - iconSize, buffY, iconSize, iconSize}, {0,0}, 0.0f, WHITE);
                    buffY -= (iconSize + 10);
                }
            }
        }
    };

    drawPlayerHUD(p1, true);
    
    if (inventory) {
        std::string coins = "Coins: " + std::to_string(inventory->coins);
        std::string keys = "Keys: " + std::to_string(inventory->keys);
        int centerX = screenWidth / 2 - 100;
        DrawTextEx(customFont, coins.c_str(), {(float)centerX, 40}, 40, 1.0f, GOLD);
        DrawTextEx(customFont, keys.c_str(), {(float)centerX, 90}, 40, 1.0f, LIGHTGRAY);
    }
}
