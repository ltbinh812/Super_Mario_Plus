#pragma once
#include "SaveData.h"
#include <string>

class SaveManager {
private:
    GameSaveData currentCheckpoint;
    bool hasCheckpointData = false;

    SaveManager() = default;

public:
    static SaveManager& getInstance() {
        static SaveManager instance;
        return instance;
    }

    SaveManager(const SaveManager&) = delete;
    SaveManager& operator=(const SaveManager&) = delete;

    void setCheckpoint(const GameSaveData& data) {
        currentCheckpoint = data;
        hasCheckpointData = true;
    }

    GameSaveData getCheckpoint() const {
        return currentCheckpoint;
    }

    bool hasCheckpoint() const {
        return hasCheckpointData;
    }

    void clearCheckpoint() {
        hasCheckpointData = false;
        currentCheckpoint = GameSaveData{};
    }

    bool saveToFile(const std::string& filepath);
    bool loadFromFile(const std::string& filepath);
};
