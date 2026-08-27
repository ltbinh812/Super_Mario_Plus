#pragma once
#include "DialogueData.h"
#include <string>
#include <unordered_map>

/**
 * DialogueRegistry — Singleton quản lý tập trung tất cả DialogueSequence đã load.
 *
 * Tại sao Singleton?
 *   - Dialogue data cần truy cập từ nhiều nơi (CutsceneManager, NPC, trigger zones...)
 *   - Chỉ cần 1 instance duy nhất quản lý cache để tránh load trùng file
 *   - Giống cách AssetManager (đã có) hoạt động trong dự án
 *
 * OOP: Đóng gói (Encapsulation) + Singleton Pattern
 */
class DialogueRegistry {
private:
    std::unordered_map<std::string, DialogueSequence> sequences;

    DialogueRegistry() = default;

public:
    // Chặn copy (Singleton chuẩn)
    DialogueRegistry(const DialogueRegistry&) = delete;
    DialogueRegistry& operator=(const DialogueRegistry&) = delete;

    static DialogueRegistry& getInstance();

    /**
     * Load dialogue từ file JSON và cache lại.
     * Nếu ID đã tồn tại trong cache thì bỏ qua (không load lại).
     * @param filePath Đường dẫn file JSON
     */
    void loadFromFile(const std::string& filePath);

    /**
     * Lấy DialogueSequence theo ID.
     * @param id ID của dialogue (VD: "world01_intro")
     * @return Pointer đến sequence, hoặc nullptr nếu không tìm thấy
     */
    const DialogueSequence* get(const std::string& id) const;

    /**
     * Kiểm tra dialogue ID đã được load chưa.
     */
    bool has(const std::string& id) const;
};
