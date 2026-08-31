#include "SaveManager.h"
#include "FileSaveRepository.h"
#include "JsonSaveSerializer.h"
#include <iostream>

// -----------------------------------------------------------------------------
// Khởi tạo lười repository mặc định: FileSaveRepository ghi JSON vào "saves/".
//
// Vì sao lười mà không dựng ngay trong constructor? SaveManager là Singleton
// Meyers, được tạo lần đầu tiên có ai gọi getInstance() — có thể là rất sớm,
// trước cả khi thư mục làm việc của tiến trình được xác lập. Dựng lười giúp
// thao tác đĩa đầu tiên chỉ xảy ra đúng lúc gameplay thật sự cần lưu/đọc.
// -----------------------------------------------------------------------------
ISaveRepository& SaveManager::repo() const {
    if (!repository_) {
        repository_ = std::make_unique<FileSaveRepository>(
            std::make_unique<JsonSaveSerializer>(), "saves");
    }
    return *repository_;
}

void SaveManager::setRepository(std::unique_ptr<ISaveRepository> repository) {
    repository_ = std::move(repository);
}

// ---- API cũ: ghi/đọc một đường dẫn cụ thể -----------------------------------

bool SaveManager::saveToFile(const std::string& filepath) {
    if (!hasCheckpointData || !currentCheckpoint.isValid) return false;

    JsonSaveSerializer serializer;
    return serializer.write(currentCheckpoint, filepath);
}

bool SaveManager::loadFromFile(const std::string& filepath) {
    JsonSaveSerializer serializer;
    GameSaveData data;
    if (!serializer.read(filepath, data)) return false;

    setCheckpoint(data);
    std::cout << "[SaveManager] Da nap checkpoint tu " << filepath << "\n";
    return true;
}

// ---- API mới: nhiều version theo world --------------------------------------

std::vector<SaveSlotInfo> SaveManager::listVersions(int worldIndex) const {
    return repo().listVersions(worldIndex);
}

bool SaveManager::createVersion(int worldIndex, const GameSaveData& data, SaveSlotInfo& out) {
    if (!data.isValid) {
        std::cerr << "[SaveManager] Tu choi luu: GameSaveData khong hop le.\n";
        return false;
    }
    return repo().createVersion(worldIndex, data, out);
}

bool SaveManager::loadVersion(const SaveSlotInfo& slot, GameSaveData& out) const {
    return repo().loadVersion(slot, out);
}

bool SaveManager::deleteVersion(const SaveSlotInfo& slot) {
    return repo().deleteVersion(slot);
}
