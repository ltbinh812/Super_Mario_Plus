#include "ShopAsset.h"
#include "Player.h"
#include <iostream>

ShopAsset::ShopAsset(Vector2 worldPos)
    : BaseItem(worldPos, 160.0f, 160.0f) // Hitbox size 160x160 (5x5 blocks of 32px)
{
    // worldPos từ LDtk truyền vào là toạ độ góc trên-trái (top-left) của block 32x32.
    // Trung tâm của block đó:
    // worldPos được truyền vào chính là góc Bottom-Left của hitbox 32x32 trong thế giới game.
    // Tức là worldPos.x = Left, worldPos.y = Bottom.
    // Chúng ta muốn cạnh đáy của Shop 160x160 (rộng 5 block, cao 5 block) nằm ngay trên mặt đất.
    // Theo tính toán thực tế, cần đặt y = worldPos.y để ảnh rơi đúng mặt đất (không bị nổi).
    // X cần dịch sang trái 2 block (64px) để block chứa pivot nằm giữa.
    worldStats.position.x = worldPos.x - 64.0f; 
    worldStats.position.y = worldPos.y;
    worldStats.startPosition = worldStats.position;

    // Đường dẫn TƯƠNG ĐỐI: CMake copy cả thư mục assets/ sang cạnh file .exe
    // sau khi build, nên đường dẫn tuyệt đối kiểu "d:/Git/..." chỉ chạy được
    // trên đúng một máy.
    std::string texPath = "assets/maps/item/shop_asset.png";
    if (FileExists(texPath.c_str())) {
        shopTex_ = LoadTexture(texPath.c_str());
    } else {
        std::cerr << "[ShopAsset] Khong tim thay texture tai: " << texPath << "\n";
    }
    
    // Đặt delay để tránh việc player vừa spawn ra chạm ngay vào shop mà vô tình mua
    setPickupDelay(0.0f);
}

ShopAsset::~ShopAsset() {
    if (shopTex_.id != 0) {
        UnloadTexture(shopTex_);
    }
}

void ShopAsset::update(float dt) {
    BaseItem::update(dt);
}

void ShopAsset::process(const std::vector<class Player*>& players) {
    // Override BaseItem's empty process to detect player proximity
    isPlayerNear_ = false;
    for (auto* player : players) {
        if (!player || player->getRuntimeStats().health <= 0) continue;

        // Tăng vùng kiểm tra va chạm lên một chút so với hitbox gốc để hiện chữ dễ hơn
        Rectangle checkRect = getHitbox();
        checkRect.x -= 10.0f;
        checkRect.width += 20.0f;
        checkRect.y -= 10.0f;
        checkRect.height += 20.0f;

        if (CheckCollisionRecs(player->getHitbox(), checkRect)) {
            isPlayerNear_ = true;
            if (IsKeyPressed(KEY_SPACE)) {
                requestOpenShop_ = true;
            }
            break;
        }
    }
}

void ShopAsset::render(float alpha) {
    // Vẽ Shop Asset. Lấy vị trí hitbox làm tham chiếu.
    // worldPos (hoặc renderX, renderY của Entity) thường là toạ độ căn giữa theo chiều X và Y.
    // Tuy nhiên theo thiết kế, ta sẽ lấy hitbox làm gốc để vẽ.
    Rectangle hb = getHitbox();
    
    // Theo yêu cầu: "tôi cần ngôi nhà shop to kích thước 3x3 sau khi hiện thị, trong ldtk 1 block là 8 hoặc 16, trong game là 32 (tôi cần 3x3 theo 32px)."
    float renderWidth = 160.0f;
    float renderHeight = 160.0f;

    // Giả sử worldPos.x và worldPos.y là toạ độ ô được chỉ định trong LDtk. 
    // Chúng ta vẽ shopTex_ lên vùng 48x48 có đáy nằm ở worldPos.y + 16 (tuỳ thuộc vào pivot LDtk).
    // Ở đây ta đơn giản hoá bằng cách vẽ dựa trên Hitbox vì Hitbox đã được khởi tạo bằng worldPos.
    if (shopTex_.id != 0) {
        Rectangle source = {0.0f, 0.0f, (float)shopTex_.width, (float)shopTex_.height};
        Rectangle dest = {
            hb.x + hb.width / 2.0f, // Center X
            hb.y + hb.height,       // Bottom Y
            renderWidth,
            renderHeight
        };
        Vector2 origin = { renderWidth / 2.0f, renderHeight }; // Pivot ở giữa đáy
        
        DrawTexturePro(shopTex_, source, dest, origin, 0.0f, WHITE);
    } else {
        // Vẽ placeholder nếu không load được ảnh
        DrawRectangleRec(hb, PURPLE);
    }

    // Vẽ chỉ dẫn nút bấm
    if (isPlayerNear_) {
        const char* text = "Press [Space] to buy";
        int fontSize = 10;
        int textWidth = MeasureText(text, fontSize);
        Vector2 textPos = {
            hb.x + hb.width / 2.0f - textWidth / 2.0f,
            hb.y - 20.0f
        };
        DrawText(text, (int)textPos.x, (int)textPos.y, fontSize, WHITE);
    }
}

void ShopAsset::onInteract(Entity& other) {
    // Không làm gì cả khi chỉ đi ngang qua (tắt Auto-open)
}

void ShopAsset::forceInteract(Entity& other) {
    // Kiểm tra xem thực thể tương tác có phải là Player không
    Player* p = dynamic_cast<Player*>(&other);
    if (p) {
        // Bật cờ để BaseLevelState biết và mở UI (khi bấm Space/E)
        requestOpenShop_ = true;
    }
}
