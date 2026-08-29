#pragma once
#include "EditorBlockDef.h"
#include <string>
#include <vector>
#include <unordered_map>

// Singleton Registry chứa toàn bộ block definitions.
//
// Design: Flyweight + Singleton
// - Khởi tạo 1 lần duy nhất khi vào MapEditorState.
// - Tất cả nơi cần đọc block def chỉ cần gọi getInstance().get("DIRT_SUNNY").
// - Không copy data, chỉ trả về const reference → tiết kiệm bộ nhớ.
class EditorBlockRegistry {
public:
    static EditorBlockRegistry& getInstance();

    // Khởi tạo toàn bộ block defs (gọi 1 lần, thường trong MapEditorState ctor).
    void init();

    // Trả về def của block. Ném ngoại lệ nếu id không tồn tại.
    const EditorBlockDef& get(const std::string& blockId) const;

    // Kiểm tra block id có tồn tại không.
    bool has(const std::string& blockId) const;

    // Lấy tất cả blockIds thuộc category nhất định (e.g. "Solid").
    std::vector<std::string> getAllInCategory(const std::string& category) const;

    // Lấy danh sách tất cả category theo thứ tự hiển thị trong UI.
    std::vector<std::string> getOrderedCategories() const;

    // Lấy CollisionType của một blockId (shorthand).
    CollisionType getCollision(const std::string& blockId) const;

private:
    EditorBlockRegistry() = default;
    EditorBlockRegistry(const EditorBlockRegistry&) = delete;
    EditorBlockRegistry& operator=(const EditorBlockRegistry&) = delete;

    // Đăng ký 1 block vào registry.
    void add(EditorBlockDef def);

    std::unordered_map<std::string, EditorBlockDef> defs_;
    std::vector<std::string>                        orderedCategories_;
};
