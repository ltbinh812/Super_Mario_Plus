#pragma once
#include "GameState.h"
#include "IrisTransition.h"
#include "WorldActionPanel.h"
#include "SaveVersionPanel.h"
#include "GameSaveData.h"
#include <raylib.h>
#include <memory>
#include <vector>

struct MapNode {
    Texture2D tex;
    Vector2 position;
    float baseScale;
    float currentScale;
    float targetScale;
    Rectangle hitBox;
    int worldIndex;
    bool isHovered;
    bool isClicked;
};

enum class MapSelectionMode {
    SinglePlayer,
    PvP
};

// Việc sẽ làm sau khi hiệu ứng chuyển cảnh (iris) đóng xong. Cần một cờ tường
// minh vì lúc transition kết thúc ta không còn biết người chơi đã bấm gì.
enum class MapSelectionAction {
    None,       // không chuyển đi đâu
    BackToMenu, // quay lại Main Menu
    NewGame,    // -> CharacterSelectionState -> world mới tinh
    LoadGame    // -> thẳng vào world, khôi phục từ pendingSave_
};

class MapSelectionState : public GameState {
private:
    Texture2D backgroundTex;
    Font customFont;
    std::unique_ptr<ITransition> transitionIn;
    std::unique_ptr<ITransition> transitionOut;
    bool isTransitioningIn;
    bool isTransitioningOut;
    
    std::vector<MapNode> mapNodes;
    int targetWorldIndex;
    MapSelectionMode currentMode;

    // Back button
    Texture2D backBtnNormal;
    Texture2D backBtnPress;
    Rectangle backBtnRect;
    bool isBackHovered;
    bool isBackPressed;
    bool isBackClicked;
    bool isReturningToMenu;

    // === Hệ thống Save/Load (chỉ dùng ở chế độ 1-Player) ===
    // Hai panel overlay: bấm world -> WorldActionPanel; bấm LOAD GAME ->
    // SaveVersionPanel. Cả hai nằm ngay trong state này thay vì tách thành
    // GameState riêng, để không phải nạp lại nền + 6 building mỗi lần hỏi.
    std::unique_ptr<WorldActionPanel> worldActionPanel_;
    std::unique_ptr<SaveVersionPanel> saveVersionPanel_;

    MapSelectionAction pendingAction_ = MapSelectionAction::None;
    GameSaveData pendingSave_;   // bản lưu đã nạp, chờ transition xong mới dùng

    void InitNodes();
    void InitSavePanels();
    // true nếu có panel đang mở -> khoá tương tác với các node phía dưới.
    bool IsAnyPanelOpen() const;

public:
    MapSelectionState(MapSelectionMode mode = MapSelectionMode::SinglePlayer);
    ~MapSelectionState() override;

    void HandleInput() override;
    void Process() override;
    void Update(float dt) override;
    void Render(float alpha) const override;
};
