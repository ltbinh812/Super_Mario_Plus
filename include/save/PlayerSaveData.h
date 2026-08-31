#pragma once
#include <string>

// =============================================================================
// PlayerSaveData — Ảnh chụp (snapshot) toàn bộ trạng thái của MỘT người chơi.
//
// Đây là một DTO (Data Transfer Object) thuần: chỉ chứa dữ liệu, không có logic,
// không include raylib/nlohmann. Nhờ vậy tầng gameplay và tầng lưu file đều dùng
// chung được mà không tạo phụ thuộc vòng.
//
// AI TẠO RA NÓ?   Player::createSaveData()   (Player tự đóng gói chính mình)
// AI ĐỌC NÓ?      Player::restoreFromSaveData(), JsonSaveSerializer, SaveVersionPanel
//
// LƯU Ý: characterName là chìa khoá để dựng lại đúng nhân vật khi Load Game —
// PlayerFactory::createPlayer(characterName, pos) tra tên này trong
// assets/config/characters.json.
// =============================================================================
struct PlayerSaveData {
    bool exists = false;             // false = khe người chơi này trống (vd: P2 ở chế độ 1P)

    std::string characterName = "";  // "Goku" | "Naruto" | "Luffy" | "Kakashi" | "Sasuke" | "Zoro"

    float posX = 0.0f;
    float posY = 0.0f;
    bool  isFacingRight = true;

    int health    = 0;
    int maxHealth = 0;
    int mana      = 0;
    int maxMana   = 0;
    int breath    = 0;               // dưỡng khí khi lặn dưới nước

    std::string storedItemSlot = ""; // Item đang cầm: "Boom", "Speed", "Shield", ...
};
