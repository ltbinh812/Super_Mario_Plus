#pragma once

// =============================================================================
// SaveLoadMode — Hộp thoại chọn slot đang phục vụ hành động nào.
//
// Tách ra file riêng theo quy tắc 1 khai báo / 1 file của dự án (trước đây nằm
// chung trong EditorSaveLoadUI.h).
//
//   Save     : ghi map đang sửa xuống slot (ghi đè nếu slot đã có)
//   Load     : nạp slot vào editor ĐỂ SỬA TIẾP
//   TestPlay : nạp slot rồi chơi thử ngay
//
// Trước đây chỉ có Save/Load, và nhánh "Load" lại đi thẳng vào màn chơi — nghĩa
// là map đã lưu KHÔNG BAO GIỜ mở lại sửa được, dù hộp thoại ghi "LOAD MAP".
// Tách TestPlay thành hành động thứ ba để mỗi nút làm đúng một việc.
// =============================================================================
enum class SaveLoadMode { Save, Load, TestPlay };
