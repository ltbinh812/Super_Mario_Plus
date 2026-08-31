#include "JsonSaveSerializer.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

using json = nlohmann::json;

// =============================================================================
// ÁNH XẠ STRUCT <-> JSON
//
// Macro NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT sinh RA CẢ HAI hàm
// to_json() và from_json() từ MỘT danh sách trường duy nhất.
//
// Đây chính là chỗ sửa tận gốc một bug cũ: SaveManager.cpp trước đây viết tay
// hai khối code riêng cho ghi và đọc, nên khi createSaveData() bổ sung `mana`
// và `storedItemSlot` thì cả hai khối đều quên -> hai trường đó bốc hơi mỗi
// lần đi qua đĩa. Với macro này, thêm một trường = sửa một chỗ, không thể lệch.
//
// Hậu tố _WITH_DEFAULT nghĩa là: trường nào thiếu trong file JSON thì lấy giá
// trị mặc định của struct thay vì ném exception -> đọc được cả file save cũ.
//
// ⚠ THỨ TỰ KHÔNG QUAN TRỌNG, nhưng TÊN TRƯỜNG thì có: đổi tên một biến thành
// viên là phá vỡ khả năng đọc mọi file save đã tồn tại.
// =============================================================================

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(
    SaveMetaData,
    schemaVersion, versionIndex, savedAtUnix, savedAtText,
    worldIndex, levelId, characterName, playTimeSeconds,
    coins, health, maxHealth)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(
    PlayerSaveData,
    exists, characterName, posX, posY, isFacingRight,
    health, maxHealth, mana, maxMana, breath, storedItemSlot)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(
    InventorySaveData,
    coins, keys)

// persistedItemStates là unordered_map<string, ItemState>: nlohmann tự chuyển
// enum class thành số nguyên (Idle=0, Active=1, Used=2).
// persistedDeadEntities là unordered_set<string>: nlohmann tự chuyển thành mảng.
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(
    LevelSaveData,
    worldId, worldIndex, levelId, mapFilePath,
    persistedItemStates, persistedDeadEntities)

// GameSaveData được viết tay (không dùng macro) vì `isValid` là cờ runtime,
// không nên nằm trong file: một file đã đọc được thì đương nhiên là hợp lệ.
static void gameSaveToJson(const GameSaveData& d, json& j) {
    j = json{
        {"meta",      d.meta},
        {"p1",        d.p1},
        {"inventory", d.inventory},
        {"levelData", d.levelData}
    };
}

static void gameSaveFromJson(const json& j, GameSaveData& d) {
    d.meta      = j.value("meta",      SaveMetaData{});
    d.p1        = j.value("p1",        PlayerSaveData{});
    d.inventory = j.value("inventory", InventorySaveData{});
    d.levelData = j.value("levelData", LevelSaveData{});
    d.isValid   = true;
}

// =============================================================================

bool JsonSaveSerializer::write(const GameSaveData& data, const std::string& filePath) const {
    json j;
    gameSaveToJson(data, j);

    std::ofstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "[JsonSaveSerializer] Khong mo duoc de ghi: " << filePath << "\n";
        return false;
    }

    file << j.dump(4);
    file.close();
    std::cout << "[JsonSaveSerializer] Da ghi save: " << filePath << "\n";
    return true;
}

bool JsonSaveSerializer::read(const std::string& filePath, GameSaveData& out) const {
    std::ifstream file(filePath);
    if (!file.is_open()) return false;

    json j;
    try {
        file >> j;
    } catch (const std::exception& e) {
        std::cerr << "[JsonSaveSerializer] File save hong (" << filePath << "): " << e.what() << "\n";
        return false;
    }

    try {
        gameSaveFromJson(j, out);
    } catch (const std::exception& e) {
        std::cerr << "[JsonSaveSerializer] Cau truc save khong hop le (" << filePath
                  << "): " << e.what() << "\n";
        return false;
    }
    return true;
}

bool JsonSaveSerializer::readMeta(const std::string& filePath, SaveMetaData& out) const {
    std::ifstream file(filePath);
    if (!file.is_open()) return false;

    json j;
    try {
        file >> j;
        out = j.value("meta", SaveMetaData{});
    } catch (const std::exception& e) {
        std::cerr << "[JsonSaveSerializer] Khong doc duoc meta (" << filePath
                  << "): " << e.what() << "\n";
        return false;
    }
    return true;
}
