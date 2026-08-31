#pragma once
#include <string>
#include <vector>

/**
 * DialogueLine — Dữ liệu cho 1 dòng hội thoại.
 *
 * Đây là struct dữ liệu thuần (Plain Old Data), không chứa logic.
 * Tách riêng data khỏi behavior (Dialogue Box xử lý render).
 *
 * OOP: Đóng gói (Encapsulation) — gom các field liên quan vào 1 struct
 */
struct DialogueLine {
    std::string speakerName;   // Tên nhân vật đang nói (VD: "Goku", "Old Man")
    std::string text;          // Nội dung câu nói
    std::string portraitKey;   // Key để tìm texture portrait qua AssetManager (VD: "goku_talk")
    float textSpeed = 30.0f;   // Tốc độ typewriter (ký tự/giây). 30 = vừa phải, 60 = nhanh
};

/**
 * DialogueSequence — Chuỗi các DialogueLine tạo thành 1 cuộc hội thoại hoàn chỉnh.
 *
 * Mỗi sequence có ID duy nhất để DialogueRegistry và CutsceneScript tham chiếu.
 */
struct DialogueSequence {
    std::string id;                     // ID duy nhất (VD: "world01_intro", "boss_taunt")
    std::vector<DialogueLine> lines;    // Danh sách các dòng hội thoại theo thứ tự
};
