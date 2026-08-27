#pragma once
#include "CutsceneScript.h"
#include "DialogueBox.h"
#include "raylib.h"
#include <string>
#include <unordered_set>

class MapCamera;

/**
 * CutscenePhase — Các giai đoạn nội bộ của cutscene.
 *
 * State machine đơn giản (enum-based) vì chỉ có 4 phase cố định.
 * Nếu sau này cần mở rộng nhiều phase phức tạp hơn, có thể refactor thành State Pattern.
 */
enum class CutscenePhase {
    Idle,       // Không có cutscene đang chạy
    PanIn,      // Camera đang pan đến target
    Dialogue,   // Hiển thị DialogueBox, chờ player đọc xong
    PanBack,    // Camera đang pan quay lại Player
    Done        // Kết thúc, sẵn sàng cleanup
};

/**
 * CutsceneManager — Orchestrator điều phối toàn bộ cutscene flow.
 *
 * Tại sao cần Orchestrator?
 *   - Cutscene = Camera Spanning + Dialogue + tạm block input
 *   - Cần 1 class trung tâm phối hợp tất cả, theo đúng Single Responsibility:
 *     + MapCamera lo di chuyển camera
 *     + DialogueBox lo hiển thị text
 *     + CutsceneManager lo PHỐI HỢP 2 thứ trên theo đúng thứ tự
 *
 * Tuân thủ 4 giai đoạn game loop:
 *   handleInput() → process() → update(dt) → render(alpha)
 *
 * OOP: Composition — sở hữu DialogueBox, tham chiếu MapCamera
 */
class CutsceneManager {
private:
    CutscenePhase phase = CutscenePhase::Idle;
    CutsceneScript currentScript;
    DialogueBox dialogueBox;
    MapCamera* camera = nullptr;       // Tham chiếu (không sở hữu), set lúc startCutscene
    Vector2 playerReturnPos = {0, 0};  // Vị trí Player để camera quay về
    float normalZoom = 1.0f;           // Zoom level ban đầu trước khi cutscene bắt đầu

    std::string finishedCutsceneId = "";
    bool justFinishedFlag = false;

    // Track các cutscene oneShot đã trigger (persist trong level)
    std::unordered_set<std::string> triggeredIds;

public:
    CutsceneManager() = default;

    /**
     * Bắt đầu cutscene.
     * @param script  Cấu hình cutscene
     * @param cam     Reference đến MapCamera của level
     * @param playerPos Vị trí Player hiện tại (để camera quay về sau)
     */
    void startCutscene(const CutsceneScript& script, MapCamera& cam, Vector2 playerPos);

    // === 4 giai đoạn game loop ===
    void handleInput();
    void process();
    void update(float dt);
    void render(float alpha) const;

    // === Truy vấn trạng thái ===
    bool isActive() const { return phase != CutscenePhase::Idle; }
    bool isTriggered(const std::string& triggerId) const;
    
    bool justFinished() const { return justFinishedFlag; }
    const std::string& getFinishedCutsceneId() const { return finishedCutsceneId; }

    // === Quản lý state persistence ===
    const std::unordered_set<std::string>& getTriggeredIds() const { return triggeredIds; }
    void setTriggeredIds(const std::unordered_set<std::string>& ids) { triggeredIds = ids; }
};
