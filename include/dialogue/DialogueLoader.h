#pragma once
#include "DialogueData.h"
#include <string>

/**
 * DialogueLoader — Chịu trách nhiệm parse file JSON thành DialogueSequence.
 *
 * Tách riêng logic đọc file khỏi logic quản lý (DialogueRegistry) và hiển thị (DialogueBox).
 * Sử dụng nlohmann/json đã có trong dự án.
 *
 * OOP: Single Responsibility — class này CHỈ lo đọc/parse file, không lo gì khác
 */
class DialogueLoader {
public:
    /**
     * Parse file JSON thành DialogueSequence.
     * @param filePath Đường dẫn file (VD: "assets/dialogues/world01_intro.json")
     * @return DialogueSequence đã parse, hoặc sequence rỗng nếu lỗi
     */
    static DialogueSequence loadFromFile(const std::string& filePath);
};
