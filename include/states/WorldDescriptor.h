#pragma once
#include "GameSaveData.h"
#include <functional>
#include <memory>
#include <string>

class GameState;

// =============================================================================
// WorldDescriptor — Hồ sơ mô tả MỘT world trong chế độ 1-Player.
//
// Gói chung 3 thứ mà trước đây nằm rải rác khắp nơi:
//   - Số hiệu + tên hiển thị          (UI cần để vẽ "WORLD 03")
//   - Đường dẫn file .ldtk            (hệ save cần để suy ra world nào)
//   - Cách dựng GameState tương ứng   (2 kiểu: chơi mới / nạp từ bản lưu)
//
// Dùng std::function thay vì con trỏ hàm để mỗi world tự quyết định class nào
// được khởi tạo (World01State, World02State, ...) mà WorldCatalog không cần
// biết. Đây là biến thể nhẹ của Abstract Factory: descriptor CHÍNH LÀ factory.
// =============================================================================
struct WorldDescriptor {
    int index = -1;                 // 1..6
    std::string displayName = "";   // "WORLD 01"
    std::string mapFilePath = "";   // "assets/maps/map01/world01.ldtk"

    // Bắt đầu ván mới với nhân vật vừa chọn ở màn Character Selection.
    std::function<std::unique_ptr<GameState>(const std::string& p1Name)> makeNew;

    // Khôi phục từ một bản lưu (nhân vật + level lấy luôn từ save).
    std::function<std::unique_ptr<GameState>(const GameSaveData& save)> makeLoaded;
};
