#pragma once

#include <raylib.h>
#include <map>
#include <string>

class SettingsManager {
public:
    static SettingsManager& GetInstance() {
        static SettingsManager instance;
        return instance;
    }

    // Prohibit copying and assignment
    SettingsManager(const SettingsManager&) = delete;
    SettingsManager& operator=(const SettingsManager&) = delete;

    void ResetToDefault();
    void ResetP1ToDefault();
    void ResetP2ToDefault();

    int GetP1Key(const std::string& action) const;
    int GetP2Key(const std::string& action) const;

    int GetP1DefaultKey(const std::string& action) const;
    int GetP2DefaultKey(const std::string& action) const;

    void SetP1Key(const std::string& action, int key);
    void SetP2Key(const std::string& action, int key);
    
    std::string GetKeyName(int key) const;

    const std::map<std::string, int>& GetAllP1Keys() const { return p1Keys_; }
    const std::map<std::string, int>& GetAllP2Keys() const { return p2Keys_; }

private:
    SettingsManager();
    ~SettingsManager() = default;

    std::map<std::string, int> p1Keys_;
    std::map<std::string, int> p2Keys_;
    
    std::map<std::string, int> p1DefaultKeys_;
    std::map<std::string, int> p2DefaultKeys_;
    
    float masterVolume_ = 1.0f;
    float musicVolume_ = 1.0f;
    float backgroundSoundVolume_ = 1.0f;
    float playerSfxVolume_ = 1.0f;
    float enemySfxVolume_ = 1.0f;
    
    bool isCreativeMode_ = false;
    
    void LoadDefaults();
public:
    void SaveToFile(const std::string& filepath = "saves/settings.json");
    void LoadFromFile(const std::string& filepath = "saves/settings.json");

    float GetMasterVolume() const { return masterVolume_; }
    void SetMasterVolume(float v) { 
        masterVolume_ = (v < 0.0f) ? 0.0f : ((v > 1.0f) ? 1.0f : v); 
        ::SetMasterVolume(masterVolume_);
        SaveToFile(); 
    }
    
    float GetMusicVolume() const { return musicVolume_; }
    void SetMusicVolume(float v);
    
    float GetBackgroundSoundVolume() const { return backgroundSoundVolume_; }
    void SetBackgroundSoundVolume(float v);
    
    float GetPlayerSFXVolume() const { return playerSfxVolume_; }
    void SetPlayerSFXVolume(float v) { playerSfxVolume_ = (v < 0.0f) ? 0.0f : ((v > 1.0f) ? 1.0f : v); SaveToFile(); }
    
    float GetEnemySFXVolume() const { return enemySfxVolume_; }
    void SetEnemySFXVolume(float v) { enemySfxVolume_ = (v < 0.0f) ? 0.0f : ((v > 1.0f) ? 1.0f : v); SaveToFile(); }
    
    bool IsCreativeMode() const { return isCreativeMode_; }
    void SetCreativeMode(bool creative) { isCreativeMode_ = creative; SaveToFile(); }
};
