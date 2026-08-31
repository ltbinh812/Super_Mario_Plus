#include "UndoRedoStack.h"

void UndoRedoStack::pushUndo(const CustomMapData& snapshot) {
    undoStack_.push_back(snapshot);
    if ((int)undoStack_.size() > MAX_HISTORY)
        undoStack_.pop_front();
    clearRedo();  // mỗi edit mới xóa redo history
}

bool UndoRedoStack::undo(CustomMapData& current) {
    if (undoStack_.empty()) return false;
    redoStack_.push_back(current);        // lưu current vào redo
    current = std::move(undoStack_.back());
    undoStack_.pop_back();
    return true;
}

bool UndoRedoStack::redo(CustomMapData& current) {
    if (redoStack_.empty()) return false;
    undoStack_.push_back(current);        // lưu current vào undo
    current = std::move(redoStack_.back());
    redoStack_.pop_back();
    return true;
}

void UndoRedoStack::clearRedo() {
    redoStack_.clear();
}
