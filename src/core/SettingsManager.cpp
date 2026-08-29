#include "SettingsManager.h"
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

SettingsManager::SettingsManager() {
    LoadDefaults();
    LoadFromFile();
}

void SettingsManager::LoadDefaults() {
    ResetP1ToDefault();
    ResetP2ToDefault();
    masterVolume_ = 1.0f;
    bgmVolume_ = 1.0f;
    playerSfxVolume_ = 1.0f;
    enemySfxVolume_ = 1.0f;
    isCreativeMode_ = false;
}

void SettingsManager::ResetToDefault() {
    LoadDefaults();
}

void SettingsManager::ResetP1ToDefault() {
    p1DefaultKeys_["Move Left"] = KEY_A;
    p1DefaultKeys_["Move Right"] = KEY_D;
    p1DefaultKeys_["Climb"] = KEY_W;
    p1DefaultKeys_["Crouch"] = KEY_S;
    p1DefaultKeys_["Jump"] = KEY_K;
    p1DefaultKeys_["Attack"] = KEY_J;
    p1DefaultKeys_["Dash"] = KEY_L;
    p1DefaultKeys_["LongAttack"] = KEY_U;
    p1DefaultKeys_["SpecialAttack"] = KEY_I;
    p1DefaultKeys_["Block"] = KEY_Q;
    p1DefaultKeys_["Interact"] = KEY_E;
    
    p1Keys_ = p1DefaultKeys_;
}

void SettingsManager::ResetP2ToDefault() {
    p2DefaultKeys_["Move Left"] = KEY_LEFT;
    p2DefaultKeys_["Move Right"] = KEY_RIGHT;
    p2DefaultKeys_["Climb"] = KEY_UP;
    p2DefaultKeys_["Crouch"] = KEY_DOWN;
    p2DefaultKeys_["Jump"] = KEY_KP_2;
    p2DefaultKeys_["Attack"] = KEY_KP_1;
    p2DefaultKeys_["Dash"] = KEY_KP_3;
    p2DefaultKeys_["LongAttack"] = KEY_KP_4;
    p2DefaultKeys_["SpecialAttack"] = KEY_KP_5;
    p2DefaultKeys_["Block"] = KEY_RIGHT_SHIFT;
    p2DefaultKeys_["Interact"] = KEY_ENTER;
    
    p2Keys_ = p2DefaultKeys_;
}

int SettingsManager::GetP1Key(const std::string& action) const {
    auto it = p1Keys_.find(action);
    if (it != p1Keys_.end()) return it->second;
    return 0;
}

int SettingsManager::GetP2Key(const std::string& action) const {
    auto it = p2Keys_.find(action);
    if (it != p2Keys_.end()) return it->second;
    return 0;
}

int SettingsManager::GetP1DefaultKey(const std::string& action) const {
    auto it = p1DefaultKeys_.find(action);
    if (it != p1DefaultKeys_.end()) return it->second;
    return 0;
}

int SettingsManager::GetP2DefaultKey(const std::string& action) const {
    auto it = p2DefaultKeys_.find(action);
    if (it != p2DefaultKeys_.end()) return it->second;
    return 0;
}

void SettingsManager::SetP1Key(const std::string& action, int key) {
    p1Keys_[action] = key;
    SaveToFile();
}

void SettingsManager::SetP2Key(const std::string& action, int key) {
    p2Keys_[action] = key;
    SaveToFile();
}

std::string SettingsManager::GetKeyName(int key) const {
    // Map some common keys to string
    if (key >= KEY_A && key <= KEY_Z) {
        return std::string(1, (char)key);
    }
    if (key >= KEY_ZERO && key <= KEY_NINE) {
        return std::string(1, (char)key);
    }
    
    switch (key) {
        case KEY_SPACE: return "Space";
        case KEY_ESCAPE: return "Esc";
        case KEY_ENTER: return "Enter";
        case KEY_TAB: return "Tab";
        case KEY_BACKSPACE: return "Backspace";
        case KEY_RIGHT: return "Right";
        case KEY_LEFT: return "Left";
        case KEY_DOWN: return "Down";
        case KEY_UP: return "Up";
        case KEY_LEFT_SHIFT: return "LShift";
        case KEY_RIGHT_SHIFT: return "RShift";
        case KEY_LEFT_CONTROL: return "LCtrl";
        case KEY_RIGHT_CONTROL: return "RCtrl";
        case KEY_LEFT_ALT: return "LAlt";
        case KEY_RIGHT_ALT: return "RAlt";
        case KEY_KP_0: return "Num 0";
        case KEY_KP_1: return "Num 1";
        case KEY_KP_2: return "Num 2";
        case KEY_KP_3: return "Num 3";
        case KEY_KP_4: return "Num 4";
        case KEY_KP_5: return "Num 5";
        case KEY_KP_6: return "Num 6";
        case KEY_KP_7: return "Num 7";
        case KEY_KP_8: return "Num 8";
        case KEY_KP_9: return "Num 9";
        case KEY_KP_DECIMAL: return "Num .";
        case KEY_KP_DIVIDE: return "Num /";
        case KEY_KP_MULTIPLY: return "Num *";
        case KEY_KP_SUBTRACT: return "Num -";
        case KEY_KP_ADD: return "Num +";
        case KEY_KP_ENTER: return "Num Enter";
        case KEY_KP_EQUAL: return "Num =";
        default: return "Unknown";
    }
}

void SettingsManager::SaveToFile(const std::string& filepath) {
    json j;
    j["p1Keys"] = p1Keys_;
    j["p2Keys"] = p2Keys_;
    j["masterVolume"] = masterVolume_;
    j["bgmVolume"] = bgmVolume_;
    j["playerSfxVolume"] = playerSfxVolume_;
    j["enemySfxVolume"] = enemySfxVolume_;
    j["creativeMode"] = isCreativeMode_;

    std::ofstream file(filepath);
    if (file.is_open()) {
        file << j.dump(4);
        file.close();
    }
}

void SettingsManager::LoadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (file.is_open()) {
        try {
            json j;
            file >> j;
            if (j.contains("p1Keys")) {
                for (auto& el : j["p1Keys"].items()) {
                    p1Keys_[el.key()] = el.value();
                }
            }
            if (j.contains("p2Keys")) {
                for (auto& el : j["p2Keys"].items()) {
                    p2Keys_[el.key()] = el.value();
                }
            }
            if (j.contains("masterVolume")) masterVolume_ = j["masterVolume"];
            if (j.contains("bgmVolume")) bgmVolume_ = j["bgmVolume"];
            if (j.contains("playerSfxVolume")) playerSfxVolume_ = j["playerSfxVolume"];
            if (j.contains("enemySfxVolume")) enemySfxVolume_ = j["enemySfxVolume"];
            if (j.contains("creativeMode")) isCreativeMode_ = j["creativeMode"];
        } catch (const std::exception& e) {
            std::cerr << "Failed to parse settings file: " << e.what() << std::endl;
        }
        file.close();
    }
}
