#pragma once
#include "WorldDescriptor.h"
#include <memory>
#include <string>
#include <vector>

class GameState;

// =============================================================================
// WorldCatalog — REGISTRY PATTERN: nơi DUY NHẤT biết "world số N là cái gì".
//
// VẤN ĐỀ NÓ GIẢI QUYẾT:
// Trước đây MapSelectionState.cpp có một khối switch(idx) 6 nhánh để ánh xạ
// worldIndex -> World0XState. Thêm luồng LOAD GAME nghĩa là phải chép nguyên
// khối đó lần thứ hai, rồi lần thứ ba khi thêm màn hình khác... Mỗi lần thêm
// world mới là phải nhớ sửa đủ mọi bản sao — đúng thứ mà rule pattern-checker
// của dự án cấm.
//
// Nay mọi tri thức về world nằm gọn trong registerWorlds() ở file .cpp. Thêm
// World07 = thêm đúng MỘT khối 6 dòng, không đụng tới UI hay hệ save.
//
// CÔNG DỤNG THỨ HAI, quan trọng không kém: indexFromMapPath() cho phép
// BaseLevelState tự hỏi "tôi đang ở world số mấy?" chỉ từ đường dẫn map. Trả
// về -1 cho menu.ldtk / pvp_map0N / custom map, nhờ đó việc auto-save khi chạm
// Flag TỰ ĐỘNG bị vô hiệu ở mọi ngữ cảnh không phải world 1-player, mà không
// cần rải if/else kiểm tra chế độ khắp nơi.
// =============================================================================
class WorldCatalog {
private:
    std::vector<WorldDescriptor> worlds_;

    WorldCatalog();            // private: chỉ getInstance() được tạo
    void registerWorlds();     // đăng ký 6 world, gọi trong constructor

public:
    static WorldCatalog& getInstance();

    WorldCatalog(const WorldCatalog&) = delete;
    WorldCatalog& operator=(const WorldCatalog&) = delete;

    // Tra hồ sơ world theo số hiệu; nullptr nếu không có.
    const WorldDescriptor* find(int index) const;

    // "assets/maps/map03/world03.ldtk" -> 3
    // Trả -1 nếu đường dẫn không thuộc world 1-player nào (menu, PvP, custom map).
    int indexFromMapPath(const std::string& mapFilePath) const;

    // Tên hiển thị cho UI; chuỗi rỗng nếu index không hợp lệ.
    std::string displayName(int index) const;

    // Dựng state chơi mới. nullptr nếu index không hợp lệ.
    std::unique_ptr<GameState> createNew(int index, const std::string& p1Name) const;

    // Dựng state khôi phục từ bản lưu. nullptr nếu index không hợp lệ.
    std::unique_ptr<GameState> createLoaded(int index, const GameSaveData& save) const;
};
