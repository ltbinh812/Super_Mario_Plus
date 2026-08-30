#pragma once
#include "ICameraMode.h"
#include "raylib.h"
#include <algorithm>
#include <string>
#include <memory>
#include <queue>

class MapCamera {
private:
    Camera2D camera;
    float zoom;
    float fixedViewportHeight; // Chuẩn cố định 416.0f (lấy Map 1 làm chuẩn)

    // Smooth Camera Properties
    Vector2 deadzoneSize;
    Vector2 logicalTarget;
    float followSpeedX;
    float followSpeedY;
    float lookAheadDistance;
    float lookAheadSpeed;
    float currentLookAheadX;
    float targetZoom;
    float zoomSpeed;

public:
    explicit MapCamera(float viewportHeight = 416.0f);
    ~MapCamera() = default;

    // Tự động tính toán lại zoom chuẩn theo viewport (lấy Map 1 làm chuẩn)
    void UpdateZoom();

    // Cập nhật vị trí camera (góc trái trên map luôn trùng góc trái trên màn hình)
    void Update(Vector2 targetPos, Vector2 playerVelocity, int mapWidth, int mapHeight, float dt = 0.0f, const Vector2* targetPos2 = nullptr);

    // Điều khiển camera thủ công cho chế độ khám phá/debug
    void MoveManual(Vector2 delta);
    void SetZoom(float newZoom) { zoom = newZoom; camera.zoom = zoom; targetZoom = newZoom; }
    void SetTargetZoom(float newTargetZoom) { targetZoom = newTargetZoom; }

    // Accessors
    float GetZoom() const { return zoom; }
    Vector2 GetTarget() const { return camera.target; }
    void SetTarget(Vector2 newTarget) { camera.target = newTarget; }
    Camera2D GetRaylibCamera() const { return camera; }

    void BeginMode() const;
    void EndMode() const;

    // === Camera Mode System (State Pattern) ===
    // Đẩy mode mới vào queue. Nếu không có mode nào đang chạy, mode mới sẽ bắt đầu ngay.
    void pushMode(std::unique_ptr<ICameraMode> mode);
    // Hủy tất cả mode trong queue và mode hiện tại, thoát cinematic
    void clearModes();
    // Gọi mỗi frame để update mode hiện tại; tự động chuyển sang mode tiếp theo khi mode hiện tại kết thúc
    void updateMode(float dt);
    // Kiểm tra camera đang ở cinematic mode (có mode nào đang chạy) hay không
    bool isCinematic() const { return isInCinematicMode; }

    // Direct setters — chỉ dành cho ICameraMode subclasses gọi (thay đổi camera internals)
    void setCameraTarget(Vector2 t) { camera.target = t; }
    void setCameraZoom(float z) { zoom = z; camera.zoom = z; }
    void setCameraOffset(Vector2 o) { camera.offset = o; }

private:
    std::unique_ptr<ICameraMode> currentMode;
    std::queue<std::unique_ptr<ICameraMode>> modeQueue;
    bool isInCinematicMode = false;

    // Chuyển sang mode tiếp theo trong queue (nếu có)
    void advanceMode();
};
