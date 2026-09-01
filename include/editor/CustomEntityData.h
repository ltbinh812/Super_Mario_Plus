#pragma once
#include <string>
#include "nlohmann/json.hpp"

// Mô tả một entity được đặt trên map trong editor.
// Tương đương với LDtkEntityData nhưng dùng grid coords thay vì pixel.
struct CustomEntityData {
    std::string      type  = "";   // e.g. "Coin", "Flag", "PlayerSpawn", "Boss_itachi"
    int              gridX = 0;    // toạ độ lưới (tile), không phải pixel
    int              gridY = 0;

    // fieldInstances dạng LDtk. BẮT BUỘC là MẢNG các {__identifier, __value}:
    // ItemFactory, EnemyFactory và CutsceneTrigger đều chỉ đọc khi is_array().
    // Lưu dạng object là mất dữ liệu âm thầm (Buff mất ItemType -> RandomBuff).
    nlohmann::json   fields = nlohmann::json::array();
};
