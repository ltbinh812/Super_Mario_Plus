#pragma once
#include <string>
#include "nlohmann/json.hpp"

// Mô tả một entity được đặt trên map trong editor.
// Tương đương với LDtkEntityData nhưng dùng grid coords thay vì pixel.
struct CustomEntityData {
    std::string      type;    // e.g. "Coin", "Flag", "PlayerSpawn"
    int              gridX;   // tọa độ lưới (tile), không phải pixel
    int              gridY;
    nlohmann::json   fields;  // fieldInstances dạng JSON (rỗng nếu không cần)
};
