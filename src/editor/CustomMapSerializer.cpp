#include "CustomMapSerializer.h"
#include "nlohmann/json.hpp"
#include <fstream>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;
using json   = nlohmann::json;

// =============================================================================
// Helpers
// =============================================================================

bool CustomMapSerializer::isValidSlot(int slot) {
    return slot >= 0 && slot < MAX_SLOTS;
}

std::string CustomMapSerializer::slotPath(int slot) {
    return "saves/custom_map_" + std::to_string(slot) + ".json";
}

bool CustomMapSerializer::slotExists(int slot) {
    if (!isValidSlot(slot)) return false;
    return fs::exists(slotPath(slot));
}

std::vector<int> CustomMapSerializer::listOccupiedSlots() {
    std::vector<int> result;
    for (int i = 0; i < MAX_SLOTS; ++i) {
        if (slotExists(i)) result.push_back(i);
    }
    return result;
}

std::string CustomMapSerializer::getSlotName(int slot) {
    if (!slotExists(slot)) return "Empty";
    try {
        std::ifstream f(slotPath(slot));
        json j; f >> j;
        std::string name = j.value("name", std::string(""));
        if (!name.empty()) return name;
    } catch (const std::exception&) {
        // File hỏng — vẫn trả về một nhãn dùng được thay vì ném ra ngoài.
    }
    return "Slot " + std::to_string(slot);
}

// =============================================================================
// Save
//
// Định dạng JSON:
// {
//   "version": 1, "name": "My Map", "width": 20, "height": 15, "tileSize": 16,
//   "tiles":    [ {"key": 380, "blockId": "DIRT_SUNNY"}, ... ],
//   "entities": [ {"type": "PlayerSpawn", "gridX": 2, "gridY": 13, "fields": []}, ... ]
// }
//
// `key` = gridY * width + gridX (thưa — chỉ lưu ô khác rỗng).
// `fields` là MẢNG (không phải object): ItemFactory/EnemyFactory/CutsceneTrigger
// đều chỉ đọc khi is_array(). Lưu dạng object là mất dữ liệu âm thầm.
// =============================================================================

bool CustomMapSerializer::save(const CustomMapData& data, int slot) {
    if (!isValidSlot(slot)) {
        std::cerr << "[CustomMapSerializer] Slot khong hop le: " << slot << "\n";
        return false;
    }

    try {
        fs::create_directories("saves");

        json j;
        j["version"]  = data.version;
        j["name"]     = data.name;
        j["width"]    = data.width;
        j["height"]   = data.height;
        j["tileSize"] = data.tileSize;

        // Sparse tiles: chỉ lưu ô khác EMPTY, và chỉ ô nằm trong biên.
        const int cellCount = data.width * data.height;
        json tilesArr = json::array();
        int skipped = 0;
        for (const auto& [key, blockId] : data.tiles) {
            if (key < 0 || key >= cellCount) { ++skipped; continue; }
            tilesArr.push_back({ {"key", key}, {"blockId", blockId} });
        }
        if (skipped > 0) {
            std::cerr << "[CustomMapSerializer] Bo qua " << skipped
                      << " tile nam ngoai bien khi luu.\n";
        }
        j["tiles"] = tilesArr;

        json entArr = json::array();
        for (const auto& e : data.entities) {
            json entry;
            entry["type"]  = e.type;
            entry["gridX"] = e.gridX;
            entry["gridY"] = e.gridY;
            // Chuẩn hoá về MẢNG — xem ghi chú ở đầu khối.
            entry["fields"] = e.fields.is_array() ? e.fields : json::array();
            entArr.push_back(entry);
        }
        j["entities"] = entArr;

        // Ghi qua file tạm rồi rename: nếu tiến trình chết giữa chừng thì file
        // slot cũ vẫn nguyên vẹn, không để lại JSON cụt mà slotExists() vẫn báo
        // là "đã có map".
        const std::string finalPath = slotPath(slot);
        const std::string tmpPath   = finalPath + ".tmp";

        {
            std::ofstream f(tmpPath, std::ios::binary | std::ios::trunc);
            if (!f.is_open()) {
                std::cerr << "[CustomMapSerializer] Khong mo duoc de ghi: " << tmpPath << "\n";
                return false;
            }
            f << j.dump(2);
            f.flush();
            if (!f.good()) {
                std::cerr << "[CustomMapSerializer] Ghi that bai: " << tmpPath << "\n";
                f.close();
                std::error_code ec; fs::remove(tmpPath, ec);
                return false;
            }
        }   // đóng file trước khi rename

        std::error_code ec;
        fs::rename(tmpPath, finalPath, ec);
        if (ec) {
            // Vài hệ thống file không cho rename đè — thử xoá đích rồi rename lại.
            fs::remove(finalPath, ec);
            fs::rename(tmpPath, finalPath, ec);
            if (ec) {
                std::cerr << "[CustomMapSerializer] Khong thay the duoc file dich: "
                          << ec.message() << "\n";
                fs::remove(tmpPath, ec);
                return false;
            }
        }

        std::cout << "[CustomMapSerializer] Da luu slot " << slot
                  << " (" << finalPath << ")\n";
        return true;

    } catch (const std::exception& ex) {
        std::cerr << "[CustomMapSerializer] Luu that bai: " << ex.what() << "\n";
        return false;
    }
}

// =============================================================================
// Load
//
// Mọi trường đều đọc bằng .value()/.contains() chứ KHÔNG dùng operator[] trên
// const json&: với nlohmann 3.11.3 + NDEBUG, operator[] const trên key thiếu sẽ
// dereference end() (hành vi không xác định) thay vì ném exception — nghĩa là
// khối try/catch bên ngoài hoàn toàn vô dụng trước file JSON thiếu trường.
// =============================================================================

CustomMapData CustomMapSerializer::load(int slot, bool* ok) {
    if (ok) *ok = false;
    CustomMapData data;

    if (!slotExists(slot)) {
        std::cerr << "[CustomMapSerializer] Slot " << slot << " khong ton tai.\n";
        return data;
    }

    try {
        std::ifstream f(slotPath(slot));
        if (!f.is_open()) {
            std::cerr << "[CustomMapSerializer] Khong mo duoc slot " << slot << "\n";
            return data;
        }

        json j; f >> j;

        // --- Kích thước: kiểm tra TRƯỚC khi dùng, vì mọi thứ phía sau phụ thuộc.
        int version  = j.value("version",  1);
        int width    = j.value("width",    20);
        int height   = j.value("height",   15);
        int tileSize = j.value("tileSize", 16);

        if (width  < MIN_DIM || width  > MAX_DIM ||
            height < MIN_DIM || height > MAX_DIM ||
            tileSize < MIN_TILE_SIZE || tileSize > MAX_TILE_SIZE) {
            std::cerr << "[CustomMapSerializer] Slot " << slot
                      << " co kich thuoc khong hop le ("
                      << width << "x" << height << ", tileSize=" << tileSize
                      << "). Tu choi nap.\n";
            return CustomMapData{};   // trả về map mặc định, ok = false
        }

        data.version  = version;
        data.name     = j.value("name", std::string("My Map"));
        data.width    = width;
        data.height   = height;
        data.tileSize = tileSize;

        // --- Tiles ---
        data.tiles.clear();
        const int cellCount = data.width * data.height;
        int badTiles = 0;
        if (j.contains("tiles") && j["tiles"].is_array()) {
            for (const auto& entry : j["tiles"]) {
                if (!entry.is_object()) { ++badTiles; continue; }
                int key = entry.value("key", -1);
                std::string blockId = entry.value("blockId", std::string(""));
                // Key phải nằm trong biên: key = gy*width+gx phụ thuộc width, nên
                // một file có width bị sửa tay sẽ sinh key vô nghĩa.
                if (key < 0 || key >= cellCount || blockId.empty()) { ++badTiles; continue; }
                data.tiles[key] = blockId;
            }
        }
        if (badTiles > 0) {
            std::cerr << "[CustomMapSerializer] Bo qua " << badTiles
                      << " tile hong/ngoai bien o slot " << slot << "\n";
        }

        // --- Entities ---
        data.entities.clear();
        int badEnts = 0;
        if (j.contains("entities") && j["entities"].is_array()) {
            for (const auto& entry : j["entities"]) {
                if (!entry.is_object()) { ++badEnts; continue; }

                CustomEntityData e;
                e.type  = entry.value("type", std::string(""));
                e.gridX = entry.value("gridX", -1);
                e.gridY = entry.value("gridY", -1);

                if (e.type.empty() || !data.inBounds(e.gridX, e.gridY)) { ++badEnts; continue; }

                // Chuẩn hoá về mảng — file cũ lưu dạng object sẽ được nâng cấp
                // âm thầm thay vì làm ItemFactory bỏ qua toàn bộ field.
                json fields = entry.value("fields", json::array());
                e.fields = fields.is_array() ? fields : json::array();

                data.entities.push_back(std::move(e));
            }
        }
        if (badEnts > 0) {
            std::cerr << "[CustomMapSerializer] Bo qua " << badEnts
                      << " entity hong/ngoai bien o slot " << slot << "\n";
        }

        std::cout << "[CustomMapSerializer] Da nap slot " << slot
                  << " — " << data.name
                  << " (" << data.width << "x" << data.height << ", "
                  << data.tiles.size() << " tile, "
                  << data.entities.size() << " entity)\n";

        if (ok) *ok = true;

    } catch (const std::exception& ex) {
        std::cerr << "[CustomMapSerializer] Nap that bai: " << ex.what() << "\n";
        // Trả về map mặc định thay vì dữ liệu nạp dở dang.
        return CustomMapData{};
    }

    return data;
}
