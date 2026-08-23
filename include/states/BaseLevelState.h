#pragma once
#include "GameState.h"
#include "TileMap.h"
#include "MapCamera.h"
#include "CombatSystem.h"
#include "InputHandler.h"
#include "Player.h"
#include "CommandQueue.h"
#include "BaseItem.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include "SaveData.h"

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

    CommandQueue spawnQueue;
    std::vector<std::unique_ptr<Entity>> activeEntities;

    std::string currentLevel;
    std::string mapFilePath;

    float respawnTimer = -1.0f;

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

    void processDeathCondition(float dt);
    void processPlayerPushing();
    void processItemInteractions();
    void processSpawnQueue();

public:
    BaseLevelState(const std::string& mapFilePath, const std::string& initialLevel = "", const std::string& p1Name = "Goku", const std::string& p2Name = "Goku");
    virtual ~BaseLevelState() = default;

    GameSaveData createSaveData() const;
    void restoreFromSaveData(const GameSaveData& data);

    void HandleInput() override;
    void Process() override;
    void Update(float dt) override;
    void Render(float alpha) const override;
};
