#pragma once
#include "ICameraMode.h"
#include "raylib.h"

/**
 * CameraFollowMode (Concrete State)
 *
 * Mode mặc định: camera bám theo 1 hoặc 2 Player.
 * Bọc lại logic đã có sẵn trong MapCamera::Update / UpdateMultiplayer.
 *
 * OOP: Kế thừa (Inheritance) từ ICameraMode
 *   - Đóng gói toàn bộ logic "follow" vào 1 class riêng
 *   - isFinished() luôn false vì mode này chạy vĩnh viễn cho đến khi bị thay thế
 */
class CameraFollowMode : public ICameraMode {
private:
    Vector2 targetPos1;
    int mapWidth;
    int mapHeight;

public:
    CameraFollowMode();

    /**
     * Cập nhật thông tin target mỗi frame (vị trí player thay đổi liên tục).
     * Phải gọi trước update() mỗi frame.
     */
    void setTarget(Vector2 pos, int mapW, int mapH);

    void update(MapCamera& cam, float dt) override;
    bool isFinished() const override;
    const char* getName() const override;
};
