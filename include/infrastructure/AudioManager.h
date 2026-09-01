#pragma once
#include <raylib.h>
#include <string>
#include <vector>

class AudioManager {
private:
    Music currentMusic;
    bool isMusicPlaying = false;
    std::string currentMusicPath = "";

    Music currentBackgroundSound;
    bool isBackgroundSoundPlaying = false;
    std::string currentBackgroundSoundPath = "";

    std::vector<std::string> musicPlaylist;
    int currentPlaylistIndex = 0;
    bool isPlaylistMode = false;

    // Private constructor
    AudioManager() {
        currentMusic.stream.buffer = nullptr;
        currentBackgroundSound.stream.buffer = nullptr;
    }

    ~AudioManager() {
        clearAll();
    }

public:
    // Chặn copy
    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

    static AudioManager& getInstance();

    // --- Quản lý Music (Menu) ---
    void PlayMusic(const std::string& filepath);
    void PlayMusicPlaylist(const std::vector<std::string>& playlist);
    void SetMusicVolume(float volume);

    // --- Quản lý Background Sound (Map) ---
    void PlayBackgroundSound(const std::string& filepath);
    void SetBackgroundSoundVolume(float volume);

    // Dừng tất cả âm thanh nền
    void StopAll();

    // Cần gọi hàm này trong Game loop để cập nhật stream âm thanh liên tục
    void Update();

    // Dọn dẹp tài nguyên
    void clearAll();
};
