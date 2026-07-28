#pragma once
#include "raylib.h"
#include <algorithm>
#include <string>

class MapCamera {
private:
    Camera2D camera;
    float zoom;
    float fixedViewportHeight; // Chuẩn cố định 416.0f (lấy Map 1 làm chuẩn)

public:
    explicit MapCamera(float viewportHeight = 416.0f);
    ~MapCamera() = default;

    // Tự động tính toán lại zoom chuẩn theo viewport (lấy Map 1 làm chuẩn)
    void UpdateZoom();

    // Cập nhật vị trí camera (góc trái trên map luôn trùng góc trái trên màn hình)
    void Update(Vector2 targetPos, int mapWidth, int mapHeight, float dt = 0.0f);
    void UpdateMultiplayer(Vector2 pos1, Vector2 pos2, int mapWidth, int mapHeight, float dt = 0.0f);

    // Điều khiển camera thủ công cho chế độ khám phá/debug
    void MoveManual(Vector2 delta);
    void SetZoom(float newZoom) { zoom = newZoom; camera.zoom = zoom; }

    // Accessors
    float GetZoom() const { return zoom; }
    Vector2 GetTarget() const { return camera.target; }
    void SetTarget(Vector2 newTarget) { camera.target = newTarget; }
    Camera2D GetRaylibCamera() const { return camera; }

    void BeginMode() const;
    void EndMode() const;
};
