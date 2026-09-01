#include "PlayerHUD.h"
#include "raylib.h"
#include "ItemAtlasRegistry.h"
#include "AssetManager.h"
#include <string>
#include <algorithm>
#include <unordered_map>

// =============================================================================
// Ảnh đại diện trên HUD — dựng từ KHUNG ĐẦU TIÊN của animation "idle" của chính
// nhân vật đang chơi.
//
// Bản cũ nạp cứng "assets/Goku_animation/avatar.png" vào một static Texture2D
// duy nhất, nên chọn Naruto hay Zoro thì HUD vẫn hiện mặt Goku, và ở chế độ hai
// người thì cả hai ô đều là Goku.
//
// Cách làm: đọc ngược sprite sheet của "idle" từ GPU, cắt khung 0, xén hết viền
// trong suốt để tìm đúng thân nhân vật, lấy một ô vuông ở PHẦN TRÊN (đầu +
// vai), phóng về 168x168 rồi bo tròn bằng mặt nạ — giống hệt xử lý của ảnh
// avatar.png cũ nên khung chân dung không phải chỉnh gì.
//
// Kết quả được nhớ theo tên nhân vật: mỗi nhân vật chỉ đọc ngược GPU một lần.
// =============================================================================
static Texture2D BuildAvatarFromIdle(const Player* p) {
    Texture2D out = {0};
    if (!p) return out;

    const Animation* idle = p->findAnimation("idle");
    if (!idle || idle->getFrameNum() <= 0) return out;

    const Texture2D& sheet = idle->getTexture();
    if (sheet.id == 0 || sheet.width <= 0 || sheet.height <= 0) return out;

    Image sheetImg = LoadImageFromTexture(sheet);
    if (sheetImg.data == nullptr) return out;

    // Khung 0: sprite sheet xếp ngang, mỗi khung rộng width/frameNum.
    int frameW = sheet.width / idle->getFrameNum();
    if (frameW <= 0) { UnloadImage(sheetImg); return out; }

    Image frame = ImageFromImage(sheetImg, Rectangle{0.0f, 0.0f, (float)frameW, (float)sheet.height});
    UnloadImage(sheetImg);
    if (frame.data == nullptr) return out;

    ImageFormat(&frame, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);

    // Xén viền trong suốt: sprite thường có nhiều khoảng trống quanh nhân vật,
    // không xén thì chân dung bị thu nhỏ tít trong khung tròn.
    ImageAlphaCrop(&frame, 0.0f);
    if (frame.width <= 0 || frame.height <= 0) { UnloadImage(frame); return out; }

    // Ô vuông ở phần trên thân: cạnh = bề rộng nhân vật (hoặc chiều cao nếu
    // thấp hơn), canh giữa theo chiều ngang, bắt đầu từ đỉnh -> lấy đầu và vai.
    int side = std::min(frame.width, frame.height);
    int srcX = (frame.width - side) / 2;
    Image portrait = ImageFromImage(frame, Rectangle{(float)srcX, 0.0f, (float)side, (float)side});
    UnloadImage(frame);
    if (portrait.data == nullptr) return out;

    ImageResize(&portrait, 168, 168);
    ImageFormat(&portrait, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);

    Image mask = GenImageColor(168, 168, BLANK);
    ImageDrawCircle(&mask, 84, 84, 84, WHITE);
    ImageAlphaMask(&portrait, mask);
    UnloadImage(mask);

    out = LoadTextureFromImage(portrait);
    UnloadImage(portrait);
    return out;
}

// Trả về ảnh đại diện của nhân vật, dựng lần đầu rồi nhớ lại.
// Texture rỗng (id == 0) nghĩa là nhân vật không có animation "idle" — người
// gọi tự bỏ qua việc vẽ.
static Texture2D GetAvatarFor(const Player* p) {
    static std::unordered_map<std::string, Texture2D> cache;
    if (!p) return Texture2D{0};

    const std::string& key = p->getBaseStats().name;
    auto it = cache.find(key);
    if (it != cache.end()) return it->second;

    Texture2D tex = BuildAvatarFromIdle(p);
    cache.emplace(key, tex);
    return tex;
}

static std::string getFrameName(const std::string& itemIdentifier) {
    if (itemIdentifier == "Boom") return "bomb.png";
    if (itemIdentifier == "Speed") return "item_speed_fix01 (Custom).png";
    if (itemIdentifier == "Strength") return "item_strength_fix01 (Custom).png";
    if (itemIdentifier == "Shield") return "item_shield_fix01 (Custom).png";
    if (itemIdentifier == "Jump") return "item_jump_fix01 (Custom).png";
    if (itemIdentifier == "Invisibility") return "item_invisibility_fix01 (Custom).png";
    if (itemIdentifier == "GoldMagnet") return "item_gold_magnet_fix01 (Custom).png";
    if (itemIdentifier == "TimeStop") return "item_time_stop_fix01 (Custom).png";
    if (itemIdentifier == "Poison") return "item_poison_fix01 (Custom).png";
    if (itemIdentifier == "Heal") return "item_heal_fix01 (Custom).png";
    return "";
}

void PlayerHUD::render(const Player* p1, const Player* p2, const PartyInventory* inventory) {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    static bool initHUD = false;
    static Texture2D frameTex = {0};
    static Font customFont = {0};
    
    if (!initHUD) {
        AssetManager::getInstance().loadTexture("hud_item_empty", "assets/maps/item/item_empty.png");
        AssetManager::getInstance().loadTexture("partyhud", "assets/maps/item/partyhud.png");
        AssetManager::getInstance().loadTexture("boom_item", "assets/maps/item/boom_item.png");

        // Ảnh đại diện KHÔNG nạp ở đây nữa: nó phụ thuộc vào nhân vật nào đang
        // chơi, nên do GetAvatarFor(p) dựng riêng cho từng người — xem đầu file.

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
            Texture2D avatarTex = GetAvatarFor(p);
            if (avatarTex.id != 0) {
                DrawTexture(avatarTex, startX + 32, avatarY + 32, WHITE);
            }
            
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
            Texture2D avatarTex = GetAvatarFor(p);
            if (avatarTex.id != 0) {
                DrawTexture(avatarTex, frameX + 32, avatarY + 32, WHITE);
            }
            
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
    if (p2) {
        drawPlayerHUD(p2, false);
    }
    
    if (inventory) {
        std::string coins = "Coins: " + std::to_string(inventory->coins);
        std::string keys = "Keys: " + std::to_string(inventory->keys);
        int centerX = screenWidth / 2 - 100;
        DrawTextEx(customFont, coins.c_str(), {(float)centerX, 40}, 40, 1.0f, GOLD);
        DrawTextEx(customFont, keys.c_str(), {(float)centerX, 90}, 40, 1.0f, LIGHTGRAY);
    }
}
