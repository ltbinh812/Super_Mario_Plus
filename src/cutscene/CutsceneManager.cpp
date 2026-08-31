#include "CutsceneManager.h"
#include "MapCamera.h"
#include "CameraPanMode.h"
#include "CameraZoomMode.h"
#include "DialogueRegistry.h"
#include <iostream>

void CutsceneManager::startCutscene(const CutsceneScript& script, MapCamera& cam, Vector2 playerPos, int mapW, int mapH) {
    // Kiểm tra oneShot: nếu đã trigger rồi thì skip
    if (script.oneShot && !script.triggerId.empty()) {
        if (triggeredIds.find(script.triggerId) != triggeredIds.end()) {
            return;
        }
    }

    // Load dialogue nếu chưa có trong registry
    const DialogueSequence* seq = DialogueRegistry::getInstance().get(script.dialogueId);
    if (!seq) {
        std::cerr << "[CutsceneManager] Dialogue '" << script.dialogueId
                  << "' not found in registry!\n";
        return;
    }

    currentScript = script;
    camera = &cam;
    playerReturnPos = playerPos;
    normalZoom = cam.GetZoom(); // Capture lại mức zoom hiện tại của camera
    currentMapW = mapW;
    currentMapH = mapH;

    // Đánh dấu oneShot đã trigger
    if (script.oneShot && !script.triggerId.empty()) {
        triggeredIds.insert(script.triggerId);
    }

    // std::cout << "[CutsceneManager] Starting cutscene: " << script.triggerId
    //           << " (dialogue: " << script.dialogueId << ")\n";

    // Quyết định phase đầu tiên
    if (script.hasCameraPan() || script.hasCameraZoom()) {
        // Có camera action → bắt đầu với PanIn
        phase = CutscenePhase::PanIn;

        if (script.hasCameraZoom()) {
            // Zoom + Pan đồng thời
            cam.pushMode(std::make_unique<CameraZoomMode>(
                script.cameraZoomTarget, script.cameraPanTarget,
                script.panDuration, EaseType::EaseInOut, currentMapW, currentMapH
            ));
        } else {
            // Chỉ Pan
            cam.pushMode(std::make_unique<CameraPanMode>(
                script.cameraPanTarget, script.panDuration, EaseType::EaseInOut,
                currentMapW, currentMapH // mapW/mapH sẽ được xử lý bởi CameraPanMode nếu cần
            ));
        }
    } else {
        // Không có camera action → bắt đầu dialogue ngay
        phase = CutscenePhase::Dialogue;
        dialogueBox.start(seq);
    }
}

// === handleInput(): Forward input đến DialogueBox khi đang ở phase Dialogue ===
void CutsceneManager::handleInput() {
    if (phase == CutscenePhase::Dialogue) {
        dialogueBox.handleInput();
    }
    // Các phase khác (PanIn, PanBack) không nhận input từ player
}

// === process(): Kiểm tra phase transition ===
void CutsceneManager::process() {
    justFinishedFlag = false; // Reset cờ mỗi frame
    
    switch (phase) {
        case CutscenePhase::PanIn:
            // Camera pan xong → chuyển sang Dialogue
            if (camera && !camera->isCinematic()) {
                phase = CutscenePhase::Dialogue;
                const DialogueSequence* seq = DialogueRegistry::getInstance().get(currentScript.dialogueId);
                if (seq) {
                    dialogueBox.start(seq);
                } else {
                    phase = CutscenePhase::Done;
                }
            }
            break;

        case CutscenePhase::Dialogue:
            dialogueBox.process();
            if (dialogueBox.isFinished()) {
                // Dialogue xong → kiểm tra cần pan back không
                if (currentScript.hasCameraPan() || currentScript.hasCameraZoom()) {
                    phase = CutscenePhase::PanBack;
                    if (camera) {
                        // Pan camera quay về vị trí Player, đồng thời zoom out lại (nếu có zoom)
                        if (currentScript.hasCameraZoom()) {
                            camera->pushMode(std::make_unique<CameraZoomMode>(
                                normalZoom, playerReturnPos, currentScript.panDuration * 0.7f,
                                EaseType::EaseOut, currentMapW, currentMapH
                            ));
                        } else {
                            camera->pushMode(std::make_unique<CameraPanMode>(
                                playerReturnPos, currentScript.panDuration * 0.7f,
                                EaseType::EaseOut, currentMapW, currentMapH
                            ));
                        }
                    }
                } else {
                    phase = CutscenePhase::Done;
                }
            }
            break;

        case CutscenePhase::PanBack:
            // Camera đã pan xong về Player → kết thúc
            if (camera && !camera->isCinematic()) {
                phase = CutscenePhase::Done;
            }
            break;

        case CutscenePhase::Done:
            // Cleanup
            finishedCutsceneId = currentScript.triggerId;
            justFinishedFlag = true;
            phase = CutscenePhase::Idle;
            camera = nullptr;
            // std::cout << "[CutsceneManager] Cutscene finished.\n";
            break;

        case CutscenePhase::Idle:
            break;
    }
}

// === update(dt): Update camera mode + dialogue box ===
void CutsceneManager::update(float dt) {
    if (phase == CutscenePhase::PanIn || phase == CutscenePhase::PanBack) {
        if (camera) {
            camera->updateMode(dt);
        }
    }

    if (phase == CutscenePhase::Dialogue) {
        dialogueBox.update(dt);
    }
}

// === render(alpha): Render dialogue box (screen space, ngoài camera mode) ===
void CutsceneManager::render(float alpha) const {
    if (phase == CutscenePhase::Dialogue) {
        dialogueBox.render(alpha);
    }
}

bool CutsceneManager::isTriggered(const std::string& triggerId) const {
    return triggeredIds.find(triggerId) != triggeredIds.end();
}
