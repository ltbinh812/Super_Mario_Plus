#pragma once
#include "raylib.h"
#include <vector>
#include <string>

// Forward declaration
class Player;

// =============================================================================
// ShopUIPanel — Giao diện Cửa Hàng trong game.
//
// Tuân thủ quy tắc OOP và 4 giai đoạn Game Loop.
// Cung cấp các chức năng vẽ bảng hàng, item, nút mua.
// Khi mở ra, State bên dưới sẽ bị "đóng băng" bằng cách ngừng gọi update().
// =============================================================================

struct ShopItemDef {
    std::string name;
    std::string type; // "Boom", "Speed", "Strength", v.v...
    int price;
    Rectangle buyBtnBounds; // Vùng click của nút Buy — chỉ được cập nhật trong update()
    bool isHovered = false;
};

class ShopUIPanel {
private:
    bool isOpen_ = false;
    Player* buyer_ = nullptr; // Người chơi đang mua (P1)

    float screenW_ = 0.0f;
    float screenH_ = 0.0f;

    Texture2D bgTex_  = {0}; // shop1.png
    Texture2D shelfTex_ = {0}; // shop2.png
    Texture2D backBtnNormalTex_ = {0};
    Texture2D backBtnPressTex_  = {0};
    Texture2D boomTex_ = {0}; // Texture riêng cho item Bomb (không dùng atlas)

    std::vector<ShopItemDef> items_;

    // Animation & Transform state
    float bgScale_ = 1.0f;
    Rectangle bgBounds_ = {0,0,0,0};

    // Y-Offset for the falling animation (Spring/Bounce)
    float animOffsetY_ = 0.0f;
    float animVelocity_ = 0.0f;

    // Scrolling
    float scrollOffset_ = 0.0f;
    float maxScroll_    = 0.0f;
    Rectangle scrollArea_ = {0,0,0,0};

    // Nút Back
    Rectangle backBtnRect_  = {0,0,0,0};
    bool isBackHovered_  = false;
    bool isBackPressed_  = false;
    bool isBackClicked_  = false;

    // Notification
    std::string notificationMsg_ = "";
    Color notificationColor_ = WHITE;
    float notificationTimer_ = 0.0f;

    // Shelf layout precalculated in init()
    float shelfW_      = 0.0f;
    float shelfH_      = 0.0f;
    float itemSpacing_ = 0.0f;

    // -----------------------------------------------------------------------
    // Private helpers (SRP: tách nhỏ trách nhiệm)
    // -----------------------------------------------------------------------

    /// Tính toán lại scrollArea_, shelfW_, shelfH_, itemSpacing_.
    /// Gọi một lần trong init() sau khi đã load texture.
    void calculateLayout();

    /// Cập nhật buyBtnBounds cho từng item dựa trên scroll hiện tại.
    /// Gọi trong update() — KHÔNG được gọi trong render().
    void updateItemBounds();

public:
    ShopUIPanel();
    ~ShopUIPanel();

    void init(float screenWidth, float screenHeight);

    // 4 giai đoạn Game Loop
    void update(float dt, Vector2 mousePos, bool mousePressed);
    void render(float alpha = 1.0f) const;

    bool isOpen() const { return isOpen_; }

    void open(Player* buyer);
    void close();
};

