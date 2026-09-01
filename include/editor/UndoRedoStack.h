#pragma once
#include "CustomMapData.h"
#include <deque>

// Command Pattern đơn giản: lưu snapshot toàn bộ CustomMapData trước/sau thay đổi.
// Dùng deque để giới hạn kích thước (max 50 bước).
// Vì CustomMapData sparse (chỉ lưu tile khác EMPTY), mỗi snapshot rất nhẹ.
class UndoRedoStack {
public:
    static constexpr int MAX_HISTORY = 50;

    // Lưu snapshot TRƯỚC khi thực hiện thay đổi.
    // Gọi mỗi khi người dùng đặt/xóa tile (trước khi modify data).
    void pushUndo(const CustomMapData& snapshot);

    // Undo: khôi phục snapshot trước, đẩy current vào redo stack.
    // Trả về false nếu không có gì để undo.
    bool undo(CustomMapData& current);

    // Redo: áp dụng snapshot từ redo stack.
    // Trả về false nếu không có gì để redo.
    bool redo(CustomMapData& current);

    // Clear redo stack (gọi sau mỗi lần edit mới)
    void clearRedo();

    // Xoá SẠCH cả hai ngăn xếp. Bắt buộc gọi khi nạp một map khác vào editor:
    // lịch sử của map cũ áp lên map mới là vô nghĩa, một cú Ctrl+Z sẽ biến map
    // vừa mở thành map trước đó.
    void clear();

    bool canUndo() const { return !undoStack_.empty(); }
    bool canRedo() const { return !redoStack_.empty(); }

private:
    std::deque<CustomMapData> undoStack_;
    std::deque<CustomMapData> redoStack_;
};
