#pragma once
#include "CustomMapData.h"
#include <string>
#include <vector>

// Chịu trách nhiệm đọc/ghi CustomMapData sang JSON file.
//
// Lưu tại: saves/custom_map_0.json ... saves/custom_map_9.json (10 slots)
// Dùng lại nlohmann::json đã có trong dự án.
class CustomMapSerializer {
public:
    static constexpr int MAX_SLOTS = 10;

    // Lưu mapData vào slot (0–9). Tạo thư mục saves/ nếu chưa có.
    // Trả về true nếu thành công.
    static bool save(const CustomMapData& data, int slot);
    
    // Xuất ra định dạng .ldtk chuẩn để BaseLevelState (engine) đọc được
    static bool exportToLDtk(const CustomMapData& data, int slot);

    // Load từ slot. Trả về CustomMapData mặc định (rỗng) nếu lỗi.
    static CustomMapData load(int slot);

    // Kiểm tra slot có file không.
    static bool slotExists(int slot);

    // Trả về danh sách các slot đang có file.
    static std::vector<int> listOccupiedSlots();

    // Tên hiển thị của slot (lấy từ field "name" trong JSON, hoặc "Empty").
    static std::string getSlotName(int slot);

private:
    static std::string slotPath(int slot);
};
