#pragma once
#include "raylib.h"
#include <unordered_map>
#include <string>
#include <iostream>

class AssetManager {
private:
    std::unordered_map<std::string, Texture2D> textures;
    std::unordered_map<std::string, Font> fonts;
    std::unordered_map<std::string, Sound> sounds;

    // Private constructor
    AssetManager() {}

public:
    // Chặn copy
    AssetManager(const AssetManager&) = delete;
    AssetManager& operator=(const AssetManager&) = delete;

    static AssetManager& getInstance();

    // --- Quản lý Texture ---
    void loadTexture(const std::string& name, const std::string& filepath);

    // Với Raylib, các struct như Texture2D rất nhỏ (chỉ chứa ID và vài thông số),
    // nên trả về theo giá trị (value) là hoàn toàn ổn và đúng chuẩn Raylib.
    const Texture2D& getTexture(const std::string& name);

    // --- Quản lý Font ---
    void loadFont(const std::string& name, const std::string& filepath);

    const Font& getFont(const std::string& name);

    // --- Quản lý Âm thanh (Sound) ---
    void loadSound(const std::string& name, const std::string& filepath);

    const Sound& getSound(const std::string& name);
    
    // --- Dọn dẹp tài nguyên ---
    // BẮT BUỘC phải gọi hàm này trước khi CloseWindow()
    void clearAll();
};