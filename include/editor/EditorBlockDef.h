#pragma once
#include "raylib.h"
#include "TileMap.h"  // CollisionType
#include <string>

// Mô tả đầy đủ một loại block trong editor:
// - collision: giá trị CollisionType để engine physics dùng
// - category: nhóm hiển thị trong CategoryPanel (e.g. "Solid", "Water"...)
// - displayName: tên hiển thị cho người chơi
// - tilesetPath: đường dẫn đến file PNG (relative to assets/)
// - uv: vùng cắt trong tileset (pixel coordinates trong file gốc)
// - nativeTileSize: kích thước gốc (8 hoặc 16) — engine sẽ scale lên 32px
// - fallbackColor: màu vẽ khi tileset chưa load hoặc không có UV
struct EditorBlockDef {
    std::string     id;             // e.g. "DIRT_SUNNY"
    std::string     category;       // e.g. "Solid"
    std::string     displayName;    // e.g. "SunnyLand Dirt"
    CollisionType   collision;      // Solid, OneWay, Ladder, v.v.

    std::string     tilesetPath;    // "" = dùng fallbackColor
    Rectangle       uv;             // {x, y, w, h} pixel trong tileset gốc
    int             nativeTileSize; // 8 hoặc 16 — sẽ scale lên 32px khi render

    Color           fallbackColor;  // màu dự phòng (dùng khi không có tileset)

    bool            isStamp2x2 = false; // Đặt true nếu click 1 lần muốn vẽ 2x2 ô (ví dụ Tree)
};
