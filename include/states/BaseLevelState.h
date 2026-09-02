#pragma once
#include "GameState.h"
#include "TileMap.h"
#include "MapCamera.h"
#include "CombatSystem.h"
#include "InputHandler.h"
#include "Player.h"
#include "CommandQueue.h"
#include "BaseItem.h"
#include "CutsceneManager.h"
#include "CutsceneTrigger.h"
#include "IngameSettingsPanel.h"
#include "ShopUIPanel.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include "SaveData.h"

// Forward declare để tránh kéo vào toàn bộ editor headers
struct CustomMapData;

class BaseLevelState : public GameState {
protected:
    TileMap map;
    MapCamera mapCamera;
    CombatSystem combatSystem;

    std::unique_ptr<Player> player1;
    std::unique_ptr<Player> player2;
    std::shared_ptr<PartyInventory> partyInventory;

    InputHandler player1Handler;
    InputHandler player2Handler;

    // Số hiệu bảng phím mà player1Handler/player2Handler đang mang. So với
    // SettingsManager::GetBindingsRevision() mỗi frame để biết người chơi vừa
    // đổi phím và phải nạp lại — xem BaseLevelState::Process().
    unsigned int boundBindingsRevision_ = 0;

    CommandQueue spawnQueue;
    std::vector<std::unique_ptr<Entity>> activeEntities;

    std::string currentLevel;
    std::string mapFilePath;

    float respawnTimer = -1.0f;

    // === Kết thúc ván đối kháng (PvP) ===
    // Khi một bên gục, KHÔNG chuyển màn ngay mà đợi pvpEndTimer_ đếm hết, để
    // animation chết kịp chạy xong rồi mới hiện màn hình trao giải.
    // -1.0f = chưa có ai gục.
    float pvpEndTimer_ = -1.0f;
    std::string pvpWinnerName_;

    // Active item instances placed in the current level
    std::vector<std::unique_ptr<BaseItem>> activeItems;
    // State persistence: iid -> ItemState (survives level transitions within same map)
    std::unordered_map<std::string, ItemState> persistedItemStates;
    std::unordered_set<std::string> persistedDeadEntities;

    std::string nextLevelToLoad;
    std::string nextLevelDir;
    float triggerGlobalX = 0.0f;
    float triggerGlobalY = 0.0f;

    void TransitionToLevel(const std::string& nextLevel, const std::string& dir, float triggerGlobalX, float triggerGlobalY);
    void spawnEntitiesFromMap();
    void bindPlayerInputs();

    // Khởi tạo người chơi + entity + asset SAU khi `map` đã nạp xong.
    // Dùng chung cho cả map LDtk lẫn custom map — xem giải thích trong .cpp.
    void initWorldFromLoadedMap(const std::string& p1Name, const std::string& p2Name);

    // Map tự tạo không có file trên đĩa; constructor đánh dấu bằng đúng chuỗi
    // này. Nhiều nhánh (hồi sinh, checkpoint, chuyển màn) phải rẽ khác đi.
    bool isCustomMap() const { return mapFilePath == "custom"; }

    void processDeathCondition(float dt);
    void processItemInteractions();
    void processSpawnQueue();


    void processCutsceneTriggers();
    void spawnCutsceneTriggersFromMap();

    // Gọi khi người chơi chạm Flag (checkpoint). Gom về một chỗ duy nhất việc
    // đặt checkpoint trong RAM + ghi một file version mới xuống saves/world0X/.
    // Tự bỏ qua ở PvP / custom map / menu — xem chi tiết trong .cpp.
    void onCheckpointReached();

    // Đăng ký callback "Return to Save" cho IngameSettingsPanel sau khi init xong.
    void initReturnToSaveCallback();

    // Tổng thời gian đã chơi (giây), cộng dồn trong Update(). Được lưu vào
    // save để panel LOAD GAME hiển thị "đã chơi bao lâu".
    float playTimeSeconds_ = 0.0f;

    // === Cutscene System ===
    CutsceneManager cutsceneManager;
    std::vector<CutsceneTrigger> cutsceneTriggers;

    // === In-Game Settings ===
    std::unique_ptr<IngameSettingsPanel> ingameSettings_;
    bool enableIngameSettings_ = true;

    // === Shop UI ===
    std::unique_ptr<ShopUIPanel> shopUI_;

    // === Game Mode ===
    bool isPvPMode_ = false;

public:
    // Original LDtk constructor
    BaseLevelState(const std::string& mapFilePath, const std::string& initialLevel = "", const std::string& p1Name = "Goku", const std::string& p2Name = "", bool isPvPMode = false);
    
    // [NEW] Constructor cho chế độ Test Play từ MapEditorState.
    // p2Name mặc định RỖNG (không phải "Goku"): bỏ trống nghĩa là chơi một
    // mình. isPvPMode do bên gọi quyết định dựa trên dữ liệu map, thay vì tự
    // bật lên chỉ vì map có hai điểm xuất phát.
    BaseLevelState(const CustomMapData& customMap, const std::string& p1Name = "Goku",
                   const std::string& p2Name = "", bool isPvPMode = false);

    // [NEW] Constructor khôi phục từ bản lưu (luồng LOAD GAME).
    // Uỷ quyền cho constructor LDtk ở trên với nhân vật + level lấy từ save,
    // rồi gọi restoreFromSaveData() để dựng lại trạng thái map và người chơi.
    BaseLevelState(const std::string& mapFilePath, const GameSaveData& save);

    virtual ~BaseLevelState() = default;

    GameSaveData createSaveData() const;
    void restoreFromSaveData(const GameSaveData& data);

    void HandleInput() override;
    void Process() override;
    void Update(float dt) override;
    void Render(float alpha) const override;
};
