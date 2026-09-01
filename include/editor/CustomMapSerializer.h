#pragma once
#include "CustomMapData.h"
#include <string>
#include <vector>

// =============================================================================
// CustomMapSerializer — Đọc/ghi CustomMapData sang JSON.
//
// Lưu tại: saves/custom_map_0.json ... saves/custom_map_9.json (10 slot)
//
// QUY TRÌNH DỮ LIỆU CỦA DỰ ÁN (đọc kỹ trước khi sửa):
//     file .ldtk gốc  --(offline, một lần)-->  EditorBlockRegistry + extracted_rules.json
//     người chơi dựng map trong editor        -->  CustomMapData (trong RAM)
//     Save                                    -->  saves/custom_map_N.json
//     Load                                    -->  CustomMapData  -->  TileMap::LoadCustomMap
//
// Luồng KHÔNG bao giờ đi ngược lại ra file .ldtk. Trước đây class này có thêm
// một hàm exportToLDtk() dài 513 dòng ghi ra saves/custom_map_N.ldtk, nhưng
// không có một dòng code nào trong dự án nạp lại file đó — nó chỉ ghi rồi thôi.
// Hàm đó đã được gỡ bỏ (xem structure.md). Đừng thêm lại trừ khi thật sự có
// bên tiêu thụ.
// =============================================================================
class CustomMapSerializer {
public:
    static constexpr int MAX_SLOTS = 10;

    // Giới hạn hợp lệ khi nạp từ đĩa. File hỏng/sửa tay nằm ngoài khoảng này sẽ
    // bị từ chối thay vì để giá trị điên rồ lọt xuống tầng dưới (tileSize = 0
    // gây chia cho 0; width âm gây bad_alloc khi cấp phát collision layer).
    static constexpr int MIN_DIM       = 10;
    static constexpr int MAX_DIM       = 500;
    static constexpr int MIN_TILE_SIZE = 8;
    static constexpr int MAX_TILE_SIZE = 64;

    // Lưu mapData vào slot (0–9). Tạo thư mục saves/ nếu chưa có.
    // Ghi qua file tạm rồi đổi tên (atomic) để không bao giờ để lại file dở dang.
    static bool save(const CustomMapData& data, int slot);

    // Nạp từ slot. `ok` (tuỳ chọn) cho biết nạp có thành công hay không —
    // cần thiết vì giá trị trả về khi lỗi là một map mặc định hợp lệ, bên gọi
    // không thể phân biệt "slot rỗng" với "đã nạp một map trống".
    static CustomMapData load(int slot, bool* ok = nullptr);

    // Kiểm tra slot có file không.
    static bool slotExists(int slot);

    // Trả về danh sách các slot đang có file.
    static std::vector<int> listOccupiedSlots();

    // Tên hiển thị của slot (field "name" trong JSON), hoặc "Empty".
    static std::string getSlotName(int slot);

private:
    static std::string slotPath(int slot);
    // true nếu slot nằm trong [0, MAX_SLOTS). Mọi API công khai đều phải gọi.
    static bool isValidSlot(int slot);
};
