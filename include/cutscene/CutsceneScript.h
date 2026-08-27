#pragma once
#include "raylib.h"
#include <string>

/**
 * CutsceneScript — Dữ liệu cấu hình cho 1 cutscene.
 *
 * Struct thuần dữ liệu (POD), không chứa logic.
 * Được parse từ LDtk entity data hoặc tạo bằng code.
 *
 * OOP: Tách data khỏi behavior — CutsceneManager đọc script này để điều phối
 */
struct CutsceneScript {
    std::string dialogueId;          // ID tham chiếu đến DialogueSequence trong DialogueRegistry
    Vector2 cameraPanTarget = {-1, -1}; // Vị trí camera pan đến (-1,-1 = không pan)
    float cameraZoomTarget = -1.0f;  // Zoom level đích (-1 = giữ nguyên)
    float panDuration = 1.0f;        // Thời gian pan/zoom (giây)
    bool oneShot = true;             // Chỉ trigger 1 lần
    std::string triggerId;           // ID duy nhất để track trạng thái đã trigger

    // Kiểm tra có cần pan camera không
    bool hasCameraPan() const { return cameraPanTarget.x >= 0 && cameraPanTarget.y >= 0; }
    // Kiểm tra có cần zoom không
    bool hasCameraZoom() const { return cameraZoomTarget > 0.0f; }
};
