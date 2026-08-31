#pragma once
#include "ICameraMode.h"
#include "CameraPanMode.h" // Dùng EaseType

/**
 * CameraZoomMode (Concrete State)
 *
 * Zoom camera mượt đến targetZoom trong duration giây.
 * Có thể kết hợp pan đồng thời (panTarget != {-1,-1}).
 *
 * OOP: Đóng gói (Encapsulation)
 *   - Tách riêng logic zoom khỏi logic pan
 *   - Tuy nhiên có thể kết hợp cả hai trong 1 mode khi cần cinematic shot
 */
class CameraZoomMode : public ICameraMode {
private:
    float startZoom;
    float targetZoom;
    Vector2 panStart;
    Vector2 panTarget;
    bool hasPan;           // Có kết hợp pan không
    float duration;
    float elapsed;
    EaseType easeType;
    bool started;          // Đánh dấu đã capture startZoom chưa
    int mapWidth;
    int mapHeight;

    float ease(float t) const;

public:
    /**
     * @param targetZoom  Zoom level đích
     * @param duration    Thời gian zoom (giây)
     * @param easeType    Kiểu nội suy
     * @param mapW        Chiều rộng map (0 = no clamp)
     * @param mapH        Chiều cao map (0 = no clamp)
     */
    CameraZoomMode(float targetZoom, float duration, EaseType easeType = EaseType::EaseInOut, int mapW = 0, int mapH = 0);

    /**
     * Kết hợp zoom + pan đồng thời (cinematic wide shot).
     * @param targetZoom  Zoom level đích
     * @param panTarget   Vị trí pan đến
     * @param duration    Thời gian (giây)
     * @param easeType    Kiểu nội suy
     * @param mapW        Chiều rộng map (0 = no clamp)
     * @param mapH        Chiều cao map (0 = no clamp)
     */
    CameraZoomMode(float targetZoom, Vector2 panTarget, float duration,
                   EaseType easeType = EaseType::EaseInOut, int mapW = 0, int mapH = 0);

    void update(MapCamera& cam, float dt) override;
    bool isFinished() const override;
    const char* getName() const override;
};
