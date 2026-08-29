#include "CutsceneTrigger.h"
#include <iostream>

CutsceneTrigger::CutsceneTrigger(Vector2 position, Vector2 size, const nlohmann::json& fields, float scale)
    : triggerZone{position.x, position.y - size.y, size.x, size.y} {

    // Parse fieldInstances từ LDtk entity data
    // LDtk fieldInstances format: [{ "__identifier": "fieldName", "__value": value }, ...]
    if (!fields.is_array()) return;

    for (const auto& field : fields) {
        if (!field.contains("__identifier") || !field.contains("__value")) continue;

        std::string fieldName = field["__identifier"].get<std::string>();
        const auto& value = field["__value"];

        if (fieldName == "dialogueId" && value.is_string()) {
            script.dialogueId = value.get<std::string>();
        }
        else if (fieldName == "cameraPanX" && value.is_number()) {
            script.cameraPanTarget.x = value.get<float>() * scale;
        }
        else if (fieldName == "cameraPanY" && value.is_number()) {
            script.cameraPanTarget.y = value.get<float>() * scale;
        }
        else if (fieldName == "cameraZoom" && value.is_number()) {
            script.cameraZoomTarget = value.get<float>();
        }
        else if (fieldName == "panDuration" && value.is_number()) {
            script.panDuration = value.get<float>();
        }
        else if (fieldName == "oneShot" && value.is_boolean()) {
            script.oneShot = value.get<bool>();
        }
        else if (fieldName == "triggerId" && value.is_string()) {
            script.triggerId = value.get<std::string>();
        }
    }

    // Nếu không có triggerId riêng, dùng dialogueId làm fallback
    if (script.triggerId.empty()) {
        script.triggerId = script.dialogueId;
    }

    std::cout << "[CutsceneTrigger] Created trigger '" << script.triggerId
              << "' at (" << position.x << ", " << position.y
              << ") size (" << size.x << "x" << size.y << ")"
              << " dialogue: " << script.dialogueId << "\n";
}

bool CutsceneTrigger::checkTrigger(Rectangle playerHitbox) const {
    // Không trigger nếu đã kích hoạt (oneShot)
    if (script.oneShot && hasTriggered) {
        return false;
    }

    // Kiểm tra va chạm AABB
    return CheckCollisionRecs(triggerZone, playerHitbox);
}
