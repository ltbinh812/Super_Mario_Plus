#pragma once
#include "CutsceneScript.h"
#include "raylib.h"
#include "nlohmann/json.hpp"
#include <string>

/**
 * CutsceneTrigger — Vùng trigger trên map kích hoạt cutscene.
 *
 * Đại diện cho entity `CutsceneTrigger` trong LDtk.
 * Khi Player đi vào triggerZone, cutscene được kích hoạt.
 *
 * OOP: Đóng gói (Encapsulation)
 *   - Gom trigger zone + script data + trạng thái đã trigger vào 1 class
 *   - Parse từ LDtk entity data (fieldInstances)
 */
class CutsceneTrigger {
private:
    Rectangle triggerZone;       // Vùng va chạm (world coordinates)
    CutsceneScript script;       // Dữ liệu cutscene
    bool hasTriggered = false;   // Đã kích hoạt chưa (cho oneShot)

public:
    CutsceneTrigger() : triggerZone{0, 0, 0, 0} {}

    /**
     * Khởi tạo từ LDtk entity data.
     * @param position   Vị trí entity trên map
     * @param size       Kích thước trigger zone (lấy từ entity size trong LDtk)
     * @param fields     fieldInstances JSON từ LDtk
     * @param scale      Tỉ lệ scale của map để nhân tọa độ cameraPan
     */
    CutsceneTrigger(Vector2 position, Vector2 size, const nlohmann::json& fields, float scale = 1.0f);

    /**
     * Kiểm tra player có đang trong trigger zone không.
     * @param playerHitbox Hitbox của player
     * @return true nếu player overlapping VÀ cutscene chưa trigger (hoặc không phải oneShot)
     */
    bool checkTrigger(Rectangle playerHitbox) const;

    /**
     * Đánh dấu đã trigger (gọi sau khi cutscene bắt đầu).
     */
    void markTriggered() { hasTriggered = true; }

    /**
     * Reset trạng thái (cho trường hợp load lại level).
     */
    void reset() { hasTriggered = false; }

    /**
     * Render the trigger zone for debugging purposes.
     */
    void renderDebug() const {
        DrawRectangleLinesEx(triggerZone, 2.0f, PURPLE);
    }

    // Accessors
    const CutsceneScript& getScript() const { return script; }
    const std::string& getTriggerId() const { return script.triggerId; }
    bool isOneShot() const { return script.oneShot; }
    bool getHasTriggered() const { return hasTriggered; }
    void setHasTriggered(bool val) { hasTriggered = val; }
};
