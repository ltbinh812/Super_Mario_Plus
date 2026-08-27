#pragma once

class MapCamera;

/**
 * ICameraMode (Interface - State Pattern)
 * 
 * Mỗi mode đóng gói 1 hành vi camera riêng biệt.
 * MapCamera giữ con trỏ đến ICameraMode hiện tại và gọi update() mỗi frame.
 * 
 * OOP: Trừu tượng hóa (Abstraction) + Đa hình (Polymorphism)
 *   - MapCamera không cần biết chi tiết mode cụ thể
 *   - Thêm mode mới (VD: CameraShakeMode) chỉ cần tạo class mới implement interface này
 */
class ICameraMode {
public:
    virtual ~ICameraMode() = default;

    /**
     * Cập nhật logic camera mỗi frame.
     * Mode tự quản lý target, zoom, lerp... thông qua reference đến MapCamera.
     * @param cam Reference đến MapCamera để mode có thể thay đổi target/zoom
     * @param dt  Delta time (fixed timestep)
     */
    virtual void update(MapCamera& cam, float dt) = 0;

    /**
     * Kiểm tra mode đã hoàn thành chưa.
     * - CameraFollowMode: luôn false (mode vĩnh viễn)
     * - CameraPanMode: true khi đã pan đến đích
     * - CameraZoomMode: true khi đã zoom xong
     */
    virtual bool isFinished() const = 0;

    /**
     * Tên mode (dùng cho debug logging).
     */
    virtual const char* getName() const = 0;
};
