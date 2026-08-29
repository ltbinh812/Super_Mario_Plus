#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "BaseItem.h"

struct PlayerSaveData {
    bool exists = false;
    float posX = 0.0f;
    float posY = 0.0f;
    int health = 0;
    int maxHealth = 0;
    int mana = 0;
    std::string storedItemSlot = "";
    bool isFacingRight = true;
};

struct InventorySaveData {
    int keys = 0;
    int coins = 0;
};

struct LevelSaveData {
    std::string worldId;
    std::string levelId;
    std::string mapFilePath;
    std::unordered_map<std::string, ItemState> persistedItemStates;
    std::unordered_set<std::string> persistedDeadEntities;
};

struct GameSaveData {
    bool isValid = false;
    PlayerSaveData p1;
    InventorySaveData inventory;
    LevelSaveData levelData;
};
