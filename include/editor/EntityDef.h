#pragma once
#include "raylib.h"
#include <string>

// =============================================================================
// EntityDef — Data struct mô tả 1 loại entity trong editor.
//
// Lấy từ phân tích LDtk world01-06:
//  - Tất cả entities dùng item/a.png làm tileset chính
//  - PlayerSpawn không có tile → dùng fallbackColor
//
// Path tuyệt đối (relative to game exe):
//   assets/maps/item/a.png  — chứa: LuckyBlock, Bomb, Coin, Door, Flag, Key, Springs
//   assets/maps/item/buff.png
//   assets/maps/item/chest_boss.png
//   assets/maps/item/chest_normal.png
// =============================================================================
struct EntityDef {
    std::string id;           // e.g. "PlayerSpawn", "Coin"
    std::string displayName;  // e.g. "Player Spawn", "Coin"
    std::string texturePath;  // path đến file PNG (relative to exe)
    Rectangle   uv;           // UV trong file PNG (pixel coords)
    Color       fallbackColor;// màu dự phòng khi không có texture
};
