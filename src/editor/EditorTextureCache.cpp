#include "EditorTextureCache.h"
#include <raylib.h>
#include <iostream>

EditorTextureCache& EditorTextureCache::getInstance() {
    static EditorTextureCache instance;
    return instance;
}

void EditorTextureCache::loadAll(const std::vector<std::string>& paths) {
    for (const auto& path : paths) {
        if (path.empty()) continue;
        if (cache_.count(path)) continue;  // Đã load rồi

        if (FileExists(path.c_str())) {
            Texture2D tex = LoadTexture(path.c_str());
            if (tex.id != 0) {
                cache_[path] = tex;
                std::cout << "[EditorTextureCache] Loaded: " << path << "\n";
            } else {
                std::cerr << "[EditorTextureCache] FAILED to load: " << path << "\n";
            }
        } else {
            std::cerr << "[EditorTextureCache] FILE NOT FOUND: " << path << "\n";
        }
    }
}

void EditorTextureCache::ensure(const std::string& path) {
    if (path.empty() || cache_.count(path)) return;

    if (FileExists(path.c_str())) {
        Texture2D tex = LoadTexture(path.c_str());
        if (tex.id != 0) {
            cache_[path] = tex;
        }
    }
}

const Texture2D& EditorTextureCache::getOrDefault(const std::string& path) const {
    auto it = cache_.find(path);
    if (it != cache_.end()) return it->second;
    return invalid_;
}

bool EditorTextureCache::has(const std::string& path) const {
    auto it = cache_.find(path);
    return it != cache_.end() && it->second.id != 0;
}

void EditorTextureCache::unloadAll() {
    for (auto& [path, tex] : cache_) {
        if (tex.id != 0) {
            UnloadTexture(tex);
        }
    }
    cache_.clear();
    std::cout << "[EditorTextureCache] Unloaded all textures.\n";
}
