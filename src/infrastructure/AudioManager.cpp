#include "infrastructure/AudioManager.h"
#include "core/SettingsManager.h"
#include <iostream>

AudioManager& AudioManager::getInstance() {
    static AudioManager instance;
    return instance;
}

void AudioManager::PlayMusic(const std::string& filepath) {
    if (filepath.empty()) return;

    // Reset playlist mode khi gọi PlayMusic tĩnh
    isPlaylistMode = false;

    // Nếu nhạc đang phát khác bài mới, tiến hành load bài mới
    if (currentMusicPath != filepath) {
        if (isMusicPlaying) {
            StopMusicStream(currentMusic);
            UnloadMusicStream(currentMusic);
        }
        
        currentMusic = LoadMusicStream(filepath.c_str());
        if (currentMusic.stream.buffer != nullptr) {
            currentMusicPath = filepath;
            PlayMusicStream(currentMusic);
            isMusicPlaying = true;
            SetMusicVolume(SettingsManager::GetInstance().GetMusicVolume());
        } else {
            std::cerr << "[AudioManager] Error loading music: " << filepath << "\n";
            isMusicPlaying = false;
        }
    } else {
        // Nếu cùng 1 bài mà bị dừng thì phát lại
        if (currentMusic.stream.buffer != nullptr && !IsMusicStreamPlaying(currentMusic)) {
            PlayMusicStream(currentMusic);
            isMusicPlaying = true;
        }
    }

    // Khi phát Music, tự động dừng Background Sound của Map
    if (isBackgroundSoundPlaying) {
        StopMusicStream(currentBackgroundSound);
        isBackgroundSoundPlaying = false;
    }
}

void AudioManager::PlayMusicPlaylist(const std::vector<std::string>& playlist) {
    if (playlist.empty()) return;
    
    musicPlaylist = playlist;
    isPlaylistMode = true;
    
    // Pick a random song to start
    int randomIndex = std::rand() % musicPlaylist.size();
    
    // Logic tương tự PlayMusic nhưng giữ cờ isPlaylistMode = true và tắt loop
    if (currentMusicPath != musicPlaylist[randomIndex]) {
        if (isMusicPlaying) {
            StopMusicStream(currentMusic);
            UnloadMusicStream(currentMusic);
        }
        
        currentMusic = LoadMusicStream(musicPlaylist[randomIndex].c_str());
        if (currentMusic.stream.buffer != nullptr) {
            currentMusic.looping = false; // [QUAN TRỌNG] Tắt lặp mặc định
            currentMusicPath = musicPlaylist[randomIndex];
            PlayMusicStream(currentMusic);
            isMusicPlaying = true;
            SetMusicVolume(SettingsManager::GetInstance().GetMusicVolume());
        } else {
            isMusicPlaying = false;
        }
    } else {
        if (currentMusic.stream.buffer != nullptr && !IsMusicStreamPlaying(currentMusic)) {
            currentMusic.looping = false;
            PlayMusicStream(currentMusic);
            isMusicPlaying = true;
        }
    }
    
    if (isBackgroundSoundPlaying) {
        StopMusicStream(currentBackgroundSound);
        isBackgroundSoundPlaying = false;
    }
}

void AudioManager::SetMusicVolume(float volume) {
    if (isMusicPlaying && currentMusic.stream.buffer != nullptr) {
        ::SetMusicVolume(currentMusic, volume);
    }
}

void AudioManager::PlayBackgroundSound(const std::string& filepath) {
    if (filepath.empty()) return;

    // Tương tự, nếu sound đang phát khác bài mới
    if (currentBackgroundSoundPath != filepath) {
        if (isBackgroundSoundPlaying) {
            StopMusicStream(currentBackgroundSound);
            UnloadMusicStream(currentBackgroundSound);
        }
        
        currentBackgroundSound = LoadMusicStream(filepath.c_str());
        if (currentBackgroundSound.stream.buffer != nullptr) {
            currentBackgroundSoundPath = filepath;
            PlayMusicStream(currentBackgroundSound);
            isBackgroundSoundPlaying = true;
            SetBackgroundSoundVolume(SettingsManager::GetInstance().GetBackgroundSoundVolume());
        } else {
            std::cerr << "[AudioManager] Error loading background sound: " << filepath << "\n";
            isBackgroundSoundPlaying = false;
        }
    } else {
        if (currentBackgroundSound.stream.buffer != nullptr && !IsMusicStreamPlaying(currentBackgroundSound)) {
            PlayMusicStream(currentBackgroundSound);
            isBackgroundSoundPlaying = true;
        }
    }

    // Khi phát Background Sound, tự động dừng Music của Menu
    if (isMusicPlaying) {
        StopMusicStream(currentMusic);
        isMusicPlaying = false;
    }
}

void AudioManager::SetBackgroundSoundVolume(float volume) {
    if (isBackgroundSoundPlaying && currentBackgroundSound.stream.buffer != nullptr) {
        ::SetMusicVolume(currentBackgroundSound, volume);
    }
}

void AudioManager::StopAll() {
    if (isMusicPlaying) {
        StopMusicStream(currentMusic);
        isMusicPlaying = false;
    }
    if (isBackgroundSoundPlaying) {
        StopMusicStream(currentBackgroundSound);
        isBackgroundSoundPlaying = false;
    }
}

void AudioManager::Update() {
    if (isMusicPlaying && currentMusic.stream.buffer != nullptr) {
        UpdateMusicStream(currentMusic);
        
        // Kiểm tra xem bài hát có nằm trong playlist và đã phát xong chưa
        if (isPlaylistMode) {
            float played = GetMusicTimePlayed(currentMusic);
            float length = GetMusicTimeLength(currentMusic);
            
            // Nếu đã phát xong (chừa 0.1s sai số nổi)
            if (played >= length - 0.1f) {
                // Chọn bài mới ngẫu nhiên
                int randomIndex = std::rand() % musicPlaylist.size();
                
                // Tránh phát lại bài cũ nếu playlist có nhiều hơn 1 bài
                if (musicPlaylist.size() > 1 && musicPlaylist[randomIndex] == currentMusicPath) {
                    randomIndex = (randomIndex + 1) % musicPlaylist.size();
                }
                
                // Gán cờ giả để ép PlayMusicPlaylist load bài mới
                currentMusicPath = ""; 
                PlayMusicPlaylist(musicPlaylist); // Đệ quy nhẹ để load bài mới
            }
        }
    }
    if (isBackgroundSoundPlaying && currentBackgroundSound.stream.buffer != nullptr) {
        UpdateMusicStream(currentBackgroundSound);
    }
}

void AudioManager::clearAll() {
    StopAll();
    if (currentMusic.stream.buffer != nullptr) {
        UnloadMusicStream(currentMusic);
        currentMusic.stream.buffer = nullptr;
        currentMusicPath = "";
    }
    if (currentBackgroundSound.stream.buffer != nullptr) {
        UnloadMusicStream(currentBackgroundSound);
        currentBackgroundSound.stream.buffer = nullptr;
        currentBackgroundSoundPath = "";
    }
}
