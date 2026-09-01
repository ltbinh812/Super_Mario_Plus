#include "AssetManager.h"


    AssetManager& AssetManager::getInstance() {
        static AssetManager instance;
        return instance;
    }

    // --- Quản lý Texture ---
    void AssetManager::loadTexture(const std::string& name, const std::string& filepath) {
        if (textures.find(name) == textures.end()) {
            // Hàm LoadTexture yêu cầu const char*, nên cần dùng .c_str()
            Texture2D texture = LoadTexture(filepath.c_str());
            if (texture.id != 0) { // Kiểm tra xem đã load thành công chưa
                textures[name] = texture;
            } else {
                std::cerr << "[Loi] Khong the tai texture:  " << filepath << "\n";
            }
        }
    }

    // Với Raylib, các struct như Texture2D rất nhỏ (chỉ chứa ID và vài thông số),
    // nên trả về theo giá trị (value) là hoàn toàn ổn và đúng chuẩn Raylib.
    const Texture2D& AssetManager::getTexture(const std::string& name) {
        auto it = textures.find(name);
        if (it == textures.end()) {
            std::cerr << "[Loi] Texture khong ton tai: \"" << name << "\"\n";
            static Texture2D fallback = {0};
            return fallback;
        }
        return it->second;
    }

    // --- Quản lý Font ---
    void AssetManager::loadFont(const std::string& name, const std::string& filepath) {
        if (fonts.find(name) == fonts.end()) {
            Font font = LoadFont(filepath.c_str());
            if (font.texture.id != 0) {
                fonts[name] = font;
            } else {
                std::cerr << "[Loi] Khong the tai font: " << filepath << "\n";
            }
        }
    }

    const Font& AssetManager::getFont(const std::string& name) {
        auto it = fonts.find(name);
        if (it == fonts.end()) {
            std::cerr << "[Loi] Font khong ton tai: \"" << name << "\"\n";
            static Font fallback = {0};
            return fallback;
        }
        return it->second;
    }

    // --- Quản lý Âm thanh (Sound) ---
    void AssetManager::loadSound(const std::string& name, const std::string& filepath) {
        if (sounds.find(name) == sounds.end()) {
            Sound sound = LoadSound(filepath.c_str());
            // Raylib Sound struct có stream.buffer != nullptr nếu load thành công
            if (sound.stream.buffer != nullptr) { 
                sounds[name] = sound;
            } else {
                std::cerr << "[Loi] Khong the tai am thanh: " << filepath << "\n";
            }
        }
    }

    const Sound& AssetManager::getSound(const std::string& name) {
        auto it = sounds.find(name);
        if (it == sounds.end()) {
            std::cerr << "[Loi] Sound khong ton tai: \"" << name << "\"\n";
            static Sound fallback = {0};
            return fallback;
        }
        return it->second;
    }

    bool AssetManager::hasSound(const std::string& name) const {
        return sounds.find(name) != sounds.end();
    }
    
    void AssetManager::printAllSoundKeys() const {
        std::cout << "--- ASSET MANAGER SOUND KEYS ---" << std::endl;
        for (const auto& pair : sounds) {
            std::cout << pair.first << std::endl;
        }
        std::cout << "--------------------------------" << std::endl;
    }
    
    // --- Dọn dẹp tài nguyên ---
    // BẮT BUỘC phải gọi hàm này trước khi CloseWindow()
    void AssetManager::clearAll() {
        for (auto& pair : textures) {
            UnloadTexture(pair.second);
        }
        for (auto& pair : fonts) {
            UnloadFont(pair.second);
        }
        for (auto& pair : sounds) {
            UnloadSound(pair.second);
        }
        
        textures.clear();
        fonts.clear();
        sounds.clear();
    }
