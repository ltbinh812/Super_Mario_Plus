#pragma once
#include "ICameraMode.h"
#include "raylib.h"

/**
 * EaseType — kiểu nội suy di chuyển camera.
 * - Linear: tốc độ đều
 * - EaseInOut: chậm → nhanh → chậm (mượt nhất, dùng cho cutscene)
 * - EaseOut: nhanh → chậm dần (dùng khi quay về Player)
 */
enum class EaseType {
    Linear,
    EaseInOut,
    EaseOut
};

/**
 * CameraPanMode (Concrete State)
 *
 * Di chuyển camera mượt từ vị trí hiện tại đến targetPosition trong duration giây.
 * Sử dụng easing function để tạo chuyển động tự nhiên.
 *
 * OOP: Đóng gói (Encapsulation)
 *   - Mọi logic lerp/ease được gói gọn trong class
 *   - MapCamera chỉ cần gọi update(), không cần biết thuật toán nội suy
 */
class CameraPanMode : public ICameraMode {
private:
    Vector2 startPos;
    Vector2 targetPos;
    float duration;
    float elapsed;
    EaseType easeType;
    bool started;       // Đánh dấu đã capture startPos chưa
    int mapWidth;       // Ranh giới map để clamp
    int mapHeight;

    // Tính giá trị easing dựa trên t (0..1)
    float ease(float t) const;

public:
    /**
     * @param target    Vị trí đích (world coordinates)
     * @param duration  Thời gian di chuyển (giây)
     * @param easeType  Kiểu nội suy
     * @param mapW      Chiều rộng map (để clamp)
     * @param mapH      Chiều cao map (để clamp)
     */
    CameraPanMode(Vector2 target, float duration, EaseType easeType = EaseType::EaseInOut,
                  int mapW = 0, int mapH = 0);

    void update(MapCamera& cam, float dt) override;
    bool isFinished() const override;
    const char* getName() const override;
};
