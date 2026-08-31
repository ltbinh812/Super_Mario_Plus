#pragma once
#include "CustomEntityData.h"
#include <string>
#include <vector>
#include <unordered_map>

// Toàn bộ dữ liệu của 1 custom map do người chơi tự tạo.
//
// Thiết kế sparse: tiles chỉ lưu các ô KHÁC EMPTY.
// Key = gridY * width + gridX  →  blockId string (e.g. "DIRT_SUNNY")
// Lợi ích: map lớn mà thưa (nhiều ô rỗng) sẽ rất nhẹ khi lưu/copy (Undo).
struct CustomMapData {
    int         version  = 1;
    std::string name     = "My Map";
    int         width    = 20;   // số tile theo chiều ngang (min 10)
    int         height   = 15;   // số tile theo chiều dọc  (min 10)
    int         tileSize = 16;   // kích thước 1 tile trong LDtk pixel (luôn 16)
                                 // Trong game sẽ render ở 32px (scale x2)

    // Sparse tile storage: chỉ có tile khác EMPTY
    std::unordered_map<int, std::string> tiles;  // key=(gy*width+gx), val=blockId

    // Danh sách entities (PlayerSpawn, Coin, Flag, v.v.)
    std::vector<CustomEntityData> entities;

    // --- Helpers ---

    // Trả về blockId tại (gx, gy), "" nếu EMPTY
    std::string getTile(int gx, int gy) const {
        auto it = tiles.find(gy * width + gx);
        return (it != tiles.end()) ? it->second : "";
    }

    // Đặt block tại (gx, gy). blockId="" để xóa.
    void setTile(int gx, int gy, const std::string& blockId) {
        int key = gy * width + gx;
        if (blockId.empty()) {
            tiles.erase(key);
        } else {
            tiles[key] = blockId;
        }
    }

    bool inBounds(int gx, int gy) const {
        return gx >= 0 && gx < width && gy >= 0 && gy < height;
    }

    // Số lượng PlayerSpawn entities (phải đúng 2 trước khi Save/TestPlay)
    int countPlayerSpawns() const {
        int cnt = 0;
        for (const auto& e : entities)
            if (e.type == "PlayerSpawn") ++cnt;
        return cnt;
    }

    // Xóa entity tại ô grid (gx, gy) nếu có
    void eraseEntityAt(int gx, int gy) {
        entities.erase(
            std::remove_if(entities.begin(), entities.end(),
                [&](const CustomEntityData& e) {
                    return e.gridX == gx && e.gridY == gy;
                }),
            entities.end());
    }

    // Kiểm tra ô (gx, gy) đã có entity chưa
    bool hasEntityAt(int gx, int gy) const {
        for (const auto& e : entities)
            if (e.gridX == gx && e.gridY == gy) return true;
        return false;
    }
};
