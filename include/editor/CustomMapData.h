#pragma once
#include "CustomEntityData.h"
#include <algorithm>
#include <string>
#include <vector>
#include <unordered_map>

// =============================================================================
// CustomMapData — Toàn bộ dữ liệu của một map do người chơi tự tạo.
//
// THIẾT KẾ THƯA (sparse): `tiles` chỉ chứa các ô KHÁC RỖNG.
//     key = gridY * width + gridX  ->  blockId
// Map lớn nhưng thưa sẽ rất nhẹ khi lưu và khi copy (Undo chụp nguyên struct này).
//
// ⚠ KEY PHỤ THUỘC `width`. Mọi phép đổi kích thước map BẮT BUỘC phải đánh lại
// toàn bộ key (xem EditorMapResizer). Dùng keyOf()/gridOf() dưới đây thay vì tự
// viết `gy*width+gx` — công thức này trước đây bị chép tay ở 14 chỗ khác nhau.
// =============================================================================
struct CustomMapData {
    int         version  = 1;
    std::string name     = "My Map";
    int         width    = 20;   // số tile theo chiều ngang
    int         height   = 15;   // số tile theo chiều dọc
    int         tileSize = 16;   // kích thước 1 tile theo pixel LDtk
                                 // (trong game render ở 32px — scale x2)

    std::unordered_map<int, std::string> tiles;   // key=(gy*width+gx) -> blockId
    std::vector<CustomEntityData>        entities;

    // --- Quy đổi key <-> toạ độ lưới ---------------------------------------

    int keyOf(int gx, int gy) const { return gy * width + gx; }
    int gridXOf(int key)      const { return key % width; }
    int gridYOf(int key)      const { return key / width; }

    bool inBounds(int gx, int gy) const {
        return gx >= 0 && gx < width && gy >= 0 && gy < height;
    }

    int cellCount() const { return width * height; }

    // --- Truy cập tile ------------------------------------------------------

    // Trả về blockId tại (gx, gy); "" nếu rỗng hoặc ngoài biên.
    std::string getTile(int gx, int gy) const {
        if (!inBounds(gx, gy)) return "";
        auto it = tiles.find(keyOf(gx, gy));
        return (it != tiles.end()) ? it->second : "";
    }

    // Đặt block tại (gx, gy). blockId="" để xoá. Ngoài biên thì bỏ qua.
    //
    // Kiểm tra biên nằm ở ĐÂY chứ không phó mặc cho bên gọi: nếu không,
    // setTile(width, 0, X) sinh key == width, trùng đúng ô (0, 1) và âm thầm
    // ghi đè một ô hoàn toàn khác.
    void setTile(int gx, int gy, const std::string& blockId) {
        if (!inBounds(gx, gy)) return;
        int key = keyOf(gx, gy);
        if (blockId.empty()) tiles.erase(key);
        else                 tiles[key] = blockId;
    }

    // --- Thống kê entity ----------------------------------------------------

    int countEntitiesOfType(const std::string& type) const {
        int cnt = 0;
        for (const auto& e : entities)
            if (e.type == type) ++cnt;
        return cnt;
    }

    int countPlayerSpawns() const { return countEntitiesOfType("PlayerSpawn"); }

    // Số boss trên map. Nhận diện bằng tiền tố "Boss_" — đúng quy ước mà
    // BaseLevelState::spawnEntitiesFromMap() dùng để chọn EnemyFactory.
    int countBosses() const {
        int cnt = 0;
        for (const auto& e : entities)
            if (e.type.rfind("Boss_", 0) == 0) ++cnt;
        return cnt;
    }

    int countEnemies() const {
        int cnt = 0;
        for (const auto& e : entities)
            if (e.type.rfind("Boss_", 0) == 0 || e.type.rfind("Mob_", 0) == 0) ++cnt;
        return cnt;
    }

    // --- Sửa entity ---------------------------------------------------------

    void eraseEntityAt(int gx, int gy) {
        entities.erase(
            std::remove_if(entities.begin(), entities.end(),
                [&](const CustomEntityData& e) {
                    return e.gridX == gx && e.gridY == gy;
                }),
            entities.end());
    }

    bool hasEntityAt(int gx, int gy) const {
        for (const auto& e : entities)
            if (e.gridX == gx && e.gridY == gy) return true;
        return false;
    }
};
