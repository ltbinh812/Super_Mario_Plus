#pragma once
#include "raylib.h"
#include <string>
#include <unordered_map>
#include <vector>

// =============================================================================
// EditorTextureCache — Flyweight / Service Locator Pattern
//
// Quản lý lifetime của tất cả Texture2D dùng trong editor.
// Load 1 lần khi khởi tạo MapEditorState, dùng chung qua reference.
// Không copy texture, chỉ trả về const reference.
//
// Usage:
//   EditorTextureCache::getInstance().loadAll();       // trong ctor MapEditorState
//   const Texture2D& tex = cache.getOrDefault(path);  // trong render
//   EditorTextureCache::getInstance().unloadAll();     // trong dtor MapEditorState
// =============================================================================
class EditorTextureCache {
public:
    static EditorTextureCache& getInstance();

    // Load tất cả tilesetPath từ EditorBlockRegistry + EntityPalette paths.
    // Gọi 1 lần sau khi EditorBlockRegistry::init() đã xong.
    void loadAll(const std::vector<std::string>& paths);

    // Load một texture cụ thể nếu chưa có (lazy load).
    void ensure(const std::string& path);

    // Trả về texture đã load. Nếu không có → trả về {0} (invalid texture).
    const Texture2D& getOrDefault(const std::string& path) const;

    // Trả về true nếu texture đã được load thành công (id != 0).
    bool has(const std::string& path) const;

    // Giải phóng toàn bộ textures (gọi trong dtor MapEditorState).
    void unloadAll();

private:
    EditorTextureCache() = default;
    EditorTextureCache(const EditorTextureCache&) = delete;
    EditorTextureCache& operator=(const EditorTextureCache&) = delete;

    std::unordered_map<std::string, Texture2D> cache_;
    Texture2D invalid_ = {0};  // Trả về khi path không tồn tại
};
