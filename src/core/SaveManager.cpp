#include "SaveManager.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

using json = nlohmann::json;

bool SaveManager::saveToFile(const std::string& filepath) {
    if (!hasCheckpointData || !currentCheckpoint.isValid) return false;

    json j;
    j["p1"] = {
        {"exists", currentCheckpoint.p1.exists},
        {"posX", currentCheckpoint.p1.posX},
        {"posY", currentCheckpoint.p1.posY},
        {"health", currentCheckpoint.p1.health},
        {"maxHealth", currentCheckpoint.p1.maxHealth},
        {"isFacingRight", currentCheckpoint.p1.isFacingRight}
    };
    j["p2"] = {
        {"exists", currentCheckpoint.p2.exists},
        {"posX", currentCheckpoint.p2.posX},
        {"posY", currentCheckpoint.p2.posY},
        {"health", currentCheckpoint.p2.health},
        {"maxHealth", currentCheckpoint.p2.maxHealth},
        {"isFacingRight", currentCheckpoint.p2.isFacingRight}
    };
    j["inventory"] = {
        {"keys", currentCheckpoint.inventory.keys},
        {"coins", currentCheckpoint.inventory.coins}
    };
    
    json itemStates = json::object();
    for (const auto& pair : currentCheckpoint.levelData.persistedItemStates) {
        itemStates[pair.first] = static_cast<int>(pair.second);
    }
    j["levelData"]["persistedItemStates"] = itemStates;

    json deadEntities = json::array();
    for (const auto& iid : currentCheckpoint.levelData.persistedDeadEntities) {
        deadEntities.push_back(iid);
    }
    j["levelData"]["persistedDeadEntities"] = deadEntities;

    j["levelData"]["worldId"] = currentCheckpoint.levelData.worldId;
    j["levelData"]["levelId"] = currentCheckpoint.levelData.levelId;
    j["levelData"]["mapFilePath"] = currentCheckpoint.levelData.mapFilePath;

    std::ofstream file(filepath);
    if (file.is_open()) {
        file << j.dump(4);
        file.close();
        std::cout << "[SaveManager] Saved to " << filepath << "\n";
        return true;
    }
    std::cerr << "[SaveManager] Failed to open " << filepath << " for saving\n";
    return false;
}

bool SaveManager::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }

    json j;
    try {
        file >> j;
    } catch (const std::exception& e) {
        std::cerr << "[SaveManager] Failed to parse save file: " << e.what() << "\n";
        return false;
    }

    GameSaveData data;
    data.isValid = true;

    auto parsePlayer = [](const json& pj, PlayerSaveData& p) {
        p.exists = pj.value("exists", false);
        p.posX = pj.value("posX", 0.0f);
        p.posY = pj.value("posY", 0.0f);
        p.health = pj.value("health", 0);
        p.maxHealth = pj.value("maxHealth", 0);
        p.isFacingRight = pj.value("isFacingRight", true);
    };

    if (j.contains("p1")) parsePlayer(j["p1"], data.p1);
    if (j.contains("p2")) parsePlayer(j["p2"], data.p2);

    if (j.contains("inventory")) {
        data.inventory.keys = j["inventory"].value("keys", 0);
        data.inventory.coins = j["inventory"].value("coins", 0);
    }

    if (j.contains("levelData")) {
        data.levelData.worldId = j["levelData"].value("worldId", "");
        data.levelData.levelId = j["levelData"].value("levelId", "");
        data.levelData.mapFilePath = j["levelData"].value("mapFilePath", "");

        if (j["levelData"].contains("persistedItemStates")) {
            for (auto& el : j["levelData"]["persistedItemStates"].items()) {
                data.levelData.persistedItemStates[el.key()] = static_cast<ItemState>(el.value().get<int>());
            }
        }
        if (j["levelData"].contains("persistedDeadEntities")) {
            for (auto& el : j["levelData"]["persistedDeadEntities"]) {
                data.levelData.persistedDeadEntities.insert(el.get<std::string>());
            }
        }
    }

    setCheckpoint(data);
    std::cout << "[SaveManager] Loaded save from " << filepath << "\n";
    return true;
}
