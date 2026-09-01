#include "EndgameAsset.h"
#include "Player.h"
#include <iostream>

EndgameAsset::EndgameAsset(Vector2 worldPos)
    : BaseItem(worldPos, 32.0f, 128.0f), // Hitbox size 32x128 (1 block wide, 4 blocks high)
      isReached_(false)
{
    // Căn chỉnh hitbox lên trên mặt đất.
    // Theo quy ước LDtk hiện tại, worldPos.y là cạnh đáy của block hiện tại.
    // Trong game này, position.y là đáy.
    worldStats.position.x = worldPos.x;
    worldStats.position.y = worldPos.y;
    worldStats.startPosition = worldStats.position;

    // Đường dẫn TƯƠNG ĐỐI: assets/ được CMake copy sang cạnh file .exe sau khi
    // build, nên đường dẫn tuyệt đối kiểu "d:/Git/..." sẽ hỏng trên máy khác.
    // gate.gif có 10 khung, mỗi khung 100ms -> 10 fps.
    gateAnim_.Load("assets/maps/item/gate.gif", 10.0f);
}

void EndgameAsset::update(float dt) {
    BaseItem::update(dt);
    // Hoạt ảnh được tiến ở đây, KHÔNG phải trong render() — theo quy tắc 4 giai
    // đoạn của dự án, render() không được phép đổi trạng thái.
    gateAnim_.Update(dt);
}

void EndgameAsset::process(const std::vector<class Player*>& players) {
    BaseItem::process(players);
}

void EndgameAsset::render(float alpha) {
    Rectangle hb = getHitbox();

    if (gateAnim_.IsLoaded()) {
        Rectangle dest = {
            hb.x + hb.width / 2.0f,  // tâm theo trục X
            hb.y + hb.height,        // đáy theo trục Y
            kRenderWidth,
            kRenderHeight
        };
        Vector2 origin = { kRenderWidth / 2.0f, kRenderHeight }; // neo giữa-đáy
        gateAnim_.DrawPro(dest, origin);
    } else {
        DrawRectangleRec(hb, YELLOW);
    }
}

void EndgameAsset::onInteract(Entity& other) {
    Player* p = dynamic_cast<Player*>(&other);
    if (p) {
        isReached_ = true;
    }
}

void EndgameAsset::forceInteract(Entity& other) {
    onInteract(other);
}
