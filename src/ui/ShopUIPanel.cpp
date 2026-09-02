#include "ShopUIPanel.h"
#include "Player.h"
#include "CharacterStats.h"
#include "raymath.h"
#include <iostream>
#include <algorithm>
#include <cctype>
#include "ItemAtlasRegistry.h"
#include "ItemUsageFactory.h"

// Helper để chuyển chữ thành thường
static std::string toLowerString(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    return result;
}

ShopUIPanel::ShopUIPanel() {
    items_.push_back({"Boom",           "Boom",        10, {0,0,0,0}, false});
    items_.push_back({"Speed Potion",   "Speed",       14, {0,0,0,0}, false});
    items_.push_back({"Strength Buff",  "Strength",    14, {0,0,0,0}, false});
    items_.push_back({"Shield Buff",    "Shield",      16, {0,0,0,0}, false});
    items_.push_back({"Health Potion",  "Heal",        12, {0,0,0,0}, false});
    items_.push_back({"Poison Flask",   "Poison",      15, {0,0,0,0}, false});
    items_.push_back({"Gold Magnet",    "GoldMagnet",  17, {0,0,0,0}, false});
    items_.push_back({"Invisibility",   "Invisibility",18, {0,0,0,0}, false});
    items_.push_back({"Super Jump",     "Jump",        14, {0,0,0,0}, false});
    items_.push_back({"Time Stop",      "TimeStop",    20, {0,0,0,0}, false});
    items_.push_back({"Mystery Box",    "Random",      11, {0,0,0,0}, false});
}

ShopUIPanel::~ShopUIPanel() {
    if (bgTex_.id != 0)             UnloadTexture(bgTex_);
    if (shelfTex_.id != 0)          UnloadTexture(shelfTex_);
    if (backBtnNormalTex_.id != 0)  UnloadTexture(backBtnNormalTex_);
    if (backBtnPressTex_.id != 0)   UnloadTexture(backBtnPressTex_);
    if (boomTex_.id != 0)           UnloadTexture(boomTex_);
}

void ShopUIPanel::init(float screenWidth, float screenHeight) {
    screenW_ = screenWidth;
    screenH_ = screenHeight;

    bgTex_            = LoadTexture("assets/UI_screens/shop1.png");
    shelfTex_         = LoadTexture("assets/UI_screens/shop2.png");
    backBtnNormalTex_ = LoadTexture("assets/UI_screens/menu_btn_back.png");
    backBtnPressTex_  = LoadTexture("assets/UI_screens/menu_btn_back_press.png");
    boomTex_          = LoadTexture("assets/maps/item/boom_item.png");

    if (bgTex_.height > 0) {
        // Dựa trên tỉ lệ phần trăm màn hình (Ví dụ: rộng 45%, cao 80%)
        float targetWidth = screenW_ * 0.45f;
        float targetHeight = screenH_ * 0.8f;
        
        // Không dùng bgScale_ để quyết định chiều rộng nữa
        bgScale_ = targetHeight / (float)bgTex_.height; // (Chỉ giữ lại nếu có dùng ở đâu đó, nhưng tốt nhất là bỏ dần)

        bgBounds_.width  = targetWidth;
        bgBounds_.height = targetHeight;
        bgBounds_.x      = screenW_ / 2.0f - bgBounds_.width / 2.0f;
        bgBounds_.y      = screenH_ / 2.0f - bgBounds_.height / 2.0f;

        calculateLayout();
    }
}

// -----------------------------------------------------------------------
// calculateLayout — Fix Bug #1
// Dùng tỉ lệ % của bgBounds thay vì nhân bgScale_ (tránh bị teo khi ảnh dọc)
// -----------------------------------------------------------------------
void ShopUIPanel::calculateLayout() {
    float padX   = bgBounds_.width  * 0.07f;
    float padTop = bgBounds_.height * 0.12f;
    float padBot = bgBounds_.height * 0.05f;

    scrollArea_ = {
        bgBounds_.x + padX,
        bgBounds_.y + padTop,
        bgBounds_.width  - 2.0f * padX,
        bgBounds_.height - padTop - padBot
    };

    shelfW_ = scrollArea_.width * 0.92f;
    // Đặt chiều cao thanh gỗ cố định bằng 16% chiều cao của khu vực scroll thay vì theo ảnh gốc
    shelfH_ = scrollArea_.height * 0.16f; 
    
    // Fallback an toàn nếu màn hình quá hẹp
    if (shelfH_ < 50.0f) shelfH_ = 50.0f;
    
    itemSpacing_ = shelfH_ + 16.0f;
}

// -----------------------------------------------------------------------
// updateItemBounds — Fix Bug #3
// Tính buyBtnBounds trong update() — KHÔNG được làm trong render()
// -----------------------------------------------------------------------
void ShopUIPanel::updateItemBounds() {
    Rectangle curScrollArea = scrollArea_;
    curScrollArea.y += animOffsetY_;

    float startY = curScrollArea.y - scrollOffset_ + 20.0f;
    float shelfX = curScrollArea.x + curScrollArea.width / 2.0f - shelfW_ / 2.0f;

    float btnW = shelfW_ * 0.18f;   // Nút chiếm 18% chiều rộng thanh gỗ
    float btnH = shelfH_ * 0.55f;   // Nút chiếm 55% chiều cao thanh gỗ

    for (size_t i = 0; i < items_.size(); ++i) {
        float rowY = startY + (float)i * itemSpacing_;
        items_[i].buyBtnBounds = {
            shelfX + shelfW_ - btnW - shelfW_ * 0.03f,
            rowY + shelfH_ / 2.0f - btnH / 2.0f + btnH * 0.2f, // Dịch xuống một xíu để có chỗ cho text giá tiền
            btnW,
            btnH
        };
    }
}

void ShopUIPanel::open(Player* buyer) {
    isOpen_ = true;
    buyer_ = buyer;
    
    // Bắt đầu hiệu ứng rơi từ trên xuống
    animOffsetY_ = -screenH_; 
    animVelocity_ = 0.0f;
    scrollOffset_ = 0.0f;
}

void ShopUIPanel::close() {
    isOpen_ = false;
    buyer_ = nullptr;
}

void ShopUIPanel::update(float dt, Vector2 mousePos, bool mousePressed) {
    if (!isOpen_) return;

    if (notificationTimer_ > 0.0f) {
        notificationTimer_ -= dt;
    }

    // Hiệu ứng lò xo (Spring Physics)
    if (abs(animOffsetY_) > 0.1f || abs(animVelocity_) > 0.1f) {
        float stiffness = 250.0f;
        float damping = 15.0f;
        float force = -stiffness * animOffsetY_ - damping * animVelocity_;
        animVelocity_ += force * dt;
        animOffsetY_ += animVelocity_ * dt;
    } else {
        animOffsetY_ = 0.0f;
        animVelocity_ = 0.0f;
    }

    if (!buyer_) return;

    // Logic nút Back giống MapSelectionState
    float baseSize = 48.0f;
    float hoverSize = 56.0f;
    Rectangle hitBox = { 10.0f, 10.0f, hoverSize, hoverSize };
    isBackHovered_ = CheckCollisionPointRec(mousePos, hitBox);
    
    if (backBtnNormalTex_.id != 0) {
        float btnSize = isBackHovered_ ? hoverSize : baseSize;
        float offset = (hoverSize - btnSize) / 2.0f;
        backBtnRect_ = { 10.0f + offset, 10.0f + offset, btnSize, btnSize };
        
        if (isBackHovered_ && mousePressed) {
            isBackPressed_ = true;
        }
    }
    
    bool mouseReleased = IsMouseButtonReleased(MOUSE_LEFT_BUTTON);
    if (mouseReleased) {
        if (isBackHovered_ && isBackPressed_) {
            isBackClicked_ = true;
        }
        isBackPressed_ = false;
    }

    if (isBackClicked_) {
        isBackClicked_ = false;
        close();
        return;
    }

    // Click ra ngoài background để tắt shop
    // Tính toạ độ background hiện tại (đã cộng offset)
    Rectangle currentBgBounds = bgBounds_;
    currentBgBounds.y += animOffsetY_;
    
    if (mousePressed && !CheckCollisionPointRec(mousePos, currentBgBounds) && !isBackHovered_) {
        close();
        return;
    }

    // Cuộn danh sách (Mouse wheel)
    float wheelMove = GetMouseWheelMove();
    if (wheelMove != 0.0f) {
        scrollOffset_ -= wheelMove * 40.0f;
    }

    // Tính giới hạn cuộn
    float totalContentHeight = (float)items_.size() * itemSpacing_;
    maxScroll_ = totalContentHeight - scrollArea_.height;
    if (maxScroll_ < 0.0f) maxScroll_ = 0.0f;
    scrollOffset_ = std::clamp(scrollOffset_, 0.0f, maxScroll_);

    // Cập nhật vùng click nút Buy theo scroll & animation offset hiện tại
    updateItemBounds();

    // Check tương tác các mặt hàng
    Rectangle currentScrollArea = scrollArea_;
    currentScrollArea.y += animOffsetY_;

    // Chỉ check tương tác nếu con trỏ chuột nằm trong vùng scroll
    if (CheckCollisionPointRec(mousePos, currentScrollArea)) {
        for (auto& item : items_) {
            item.isHovered = CheckCollisionPointRec(mousePos, item.buyBtnBounds);

            if (item.isHovered && mousePressed) {
                auto inventory = buyer_->getPartyInventory();
                if (inventory && inventory->coins >= item.price) {
                    // Mua hàng
                    inventory->coins -= item.price;
                    std::string itemType = item.type;
                    
                    if (itemType == "Random") {
                        const auto& usableItems = ItemUsageFactory::allUsableItems();
                        itemType = usableItems[rand() % usableItems.size()];
                        std::cout << "[Shop] Mystery Box rolled: " << itemType << "!\n";
                    }
                    
                    buyer_->getRuntimeStatsMutable().storedItemSlot = itemType;
                    std::cout << "[Shop] Bought " << item.name << " for " << item.price << " coins.\n";
                    
                    notificationMsg_ = "Bought successful: " + item.name + "!";
                    notificationColor_ = GREEN;
                    notificationTimer_ = 2.0f;
                } else {
                    std::cout << "[Shop] Not enough coins to buy " << item.name << "!\n";
                    
                    notificationMsg_ = "Not enough coins to buy: " + item.name + "!";
                    notificationColor_ = RED;
                    notificationTimer_ = 2.0f;
                }
            }
        }
    } else {
        for (auto& item : items_) item.isHovered = false;
    }
}

void ShopUIPanel::render(float alpha) const {
    if (!isOpen_) return;

    // 1. Phủ màn hình lớp đen mờ (Không bị ảnh hưởng bởi hiệu ứng rơi)
    DrawRectangle(0, 0, (int)screenW_, (int)screenH_, {0, 0, 0, 150});

    // 2. Nút Back
    if (backBtnNormalTex_.id != 0) {
        Texture2D texToDraw = isBackPressed_ ? backBtnPressTex_ : backBtnNormalTex_;
        if (texToDraw.id == 0) texToDraw = backBtnNormalTex_;
        Rectangle src = { 0, 0, (float)texToDraw.width, (float)texToDraw.height };
        DrawTexturePro(texToDraw, src, backBtnRect_, {0,0}, 0.0f, WHITE);
    } else {
        DrawRectangleRec(backBtnRect_, isBackHovered_ ? LIGHTGRAY : GRAY);
        DrawRectangleLinesEx(backBtnRect_, 2.0f, DARKGRAY);
        DrawText("<- BACK", (int)backBtnRect_.x + 15, (int)backBtnRect_.y + 10, 20, WHITE);
    }

    if (bgTex_.id == 0) return;

    Rectangle currentBgBounds = bgBounds_;
    currentBgBounds.y += animOffsetY_;

    // 3. Vẽ bảng shop chính
    Rectangle srcBg = {0, 0, (float)bgTex_.width, (float)bgTex_.height};
    DrawTexturePro(bgTex_, srcBg, currentBgBounds, {0,0}, 0.0f, WHITE);

    if (buyer_ && buyer_->getPartyInventory()) {
        DrawText(TextFormat("Coins: %d", buyer_->getPartyInventory()->coins), 
                 (int)currentBgBounds.x + 30, (int)currentBgBounds.y + 30, 24, YELLOW);
    }

    // 4. Bắt đầu vùng cuộn
    Rectangle currentScrollArea = scrollArea_;
    currentScrollArea.y += animOffsetY_;
    
    BeginScissorMode((int)currentScrollArea.x, (int)currentScrollArea.y, 
                     (int)currentScrollArea.width, (int)currentScrollArea.height);

    float startY = currentScrollArea.y - scrollOffset_ + 20.0f;

    for (size_t i = 0; i < items_.size(); ++i) {
        float rowY = startY + (float)i * itemSpacing_;
        const ShopItemDef& item = items_[i];

        // --- Vẽ kệ gỗ (shop2.png) co giãn để vừa scrollArea ---
        if (shelfTex_.id != 0) {
            float shelfX = currentScrollArea.x + currentScrollArea.width / 2.0f - shelfW_ / 2.0f;

            Rectangle srcShelf  = {0, 0, (float)shelfTex_.width, (float)shelfTex_.height};
            Rectangle destShelf = {shelfX, rowY, shelfW_, shelfH_};
            DrawTexturePro(shelfTex_, srcShelf, destShelf, {0,0}, 0.0f, WHITE);

            // --- Fix Bug #2: Lấy icon — ưu tiên atlas, fallback màu debug ---
            Texture2D    iconTex = {0};
            Rectangle    srcRect = {0, 0, 0, 0};

            if (item.type == "Boom") {
                // Bomb dùng texture riêng (không có trong atlas buff)
                iconTex = boomTex_;
                srcRect = {0.0f, 0.0f, (float)boomTex_.width, (float)boomTex_.height};
            } else {
                std::string typeLower = toLowerString(item.type);
                // Fix Bug: Ánh xạ lại tên file của 2 item này vì trong atlas 
                // chúng dùng dấu gạch dưới (gold_magnet, time_stop)
                if (item.type == "GoldMagnet") typeLower = "gold_magnet";
                else if (item.type == "TimeStop") typeLower = "time_stop";

                const std::string frameName =
                    "item_" + typeLower + "_fix01 (Custom).png";
                const auto& registry = ItemAtlasRegistry::getInstance();
                if (registry.isLoaded()) {
                    const Texture2D& atlasTex = registry.getTexture(frameName);
                    if (atlasTex.id != 0) {
                        iconTex = atlasTex;
                        srcRect = registry.getFrame(frameName);
                    }
                }
            }

            // --- Vẽ icon nếu có ---
            if (iconTex.id != 0 && srcRect.width > 0 && srcRect.height > 0) {
                float shelfX2 = currentScrollArea.x + currentScrollArea.width / 2.0f - shelfW_ / 2.0f;
                float maxIconH = shelfH_ * 0.75f;
                float iconScale = maxIconH / srcRect.height;
                float renderW = srcRect.width  * iconScale;
                float renderH = srcRect.height * iconScale;
                float iconX = shelfX2 + shelfW_ * 0.04f;
                float iconY = rowY + shelfH_ / 2.0f - renderH / 2.0f;
                DrawTexturePro(iconTex, srcRect, {iconX, iconY, renderW, renderH}, {0,0}, 0.0f, WHITE);
            } else {
                // Fallback: vẽ hình chữ nhật màu để dễ debug
                float shelfX2 = currentScrollArea.x + currentScrollArea.width / 2.0f - shelfW_ / 2.0f;
                float sz = shelfH_ * 0.6f;
                DrawRectangle((int)(shelfX2 + shelfW_ * 0.04f),
                              (int)(rowY + shelfH_ / 2.0f - sz / 2.0f),
                              (int)sz, (int)sz, {180, 80, 80, 200});
            }

            // --- Vẽ tên item ---
            float shelfX3 = currentScrollArea.x + currentScrollArea.width / 2.0f - shelfW_ / 2.0f;
            float textX = shelfX3 + shelfW_ * 0.22f;
            int fontSize = (int)(shelfH_ * 0.35f);
            if (fontSize < 10) fontSize = 10;
            DrawText(item.name.c_str(),
                     (int)textX,
                     (int)(rowY + shelfH_ / 2.0f - fontSize / 2.0f),
                     fontSize, WHITE);

            // --- Vẽ nút Buy (bounds đã được update() tính sẵn) ---
            const Rectangle& btn = item.buyBtnBounds;
            Color btnColor = item.isHovered ? LIME : GREEN;
            DrawRectangleRec(btn, btnColor);
            DrawRectangleLinesEx(btn, 2.0f, DARKGREEN);

            int buyFontSize = (int)(btn.height * 0.5f);
            if (buyFontSize < 8) buyFontSize = 8;
            int buyTextW = MeasureText("BUY", buyFontSize);
            DrawText("BUY",
                     (int)(btn.x + btn.width  / 2.0f - buyTextW / 2.0f),
                     (int)(btn.y + btn.height / 2.0f - buyFontSize / 2.0f),
                     buyFontSize, WHITE);

            // --- Vẽ giá tiền bên trên nút Buy ---
            std::string priceText = std::to_string(item.price) + " C";
            int priceFontSize = (int)(btn.height * 0.45f);
            if (priceFontSize < 8) priceFontSize = 8;
            int priceW = MeasureText(priceText.c_str(), priceFontSize);
            DrawText(priceText.c_str(),
                     (int)(btn.x + btn.width / 2.0f - priceW / 2.0f),
                     (int)(btn.y - priceFontSize - 4.0f),
                     priceFontSize, YELLOW);
        }
    }

    EndScissorMode();

    if (notificationTimer_ > 0.0f) {
        DrawText(notificationMsg_.c_str(), 40, (int)(screenH_ - 80), 36, notificationColor_);
    }
}
