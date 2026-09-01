#pragma once
#include "CustomMapData.h"
#include <string>

// =============================================================================
// CustomMapValidator — Luật hợp lệ của một custom map.
//
// LUẬT (do người dùng quy định): map phải thuộc đúng MỘT trong hai thể loại
//
//     A. 1 NGƯỜI + BOSS   : đúng 1 PlayerSpawn và đúng 1 Boss_*
//     B. ĐỐI KHÁNG 2 NGƯỜI: đúng 2 PlayerSpawn và KHÔNG có Boss_*
//
// Mọi cấu hình khác đều bị từ chối kèm thông báo nêu rõ vi phạm.
//
// VÌ SAO TÁCH RA CLASS RIÊNG?
// Trước đây luật nằm lẫn trong MapEditorState::validateMapBeforeAction() và chỉ
// kiểm `spawns >= 1`, mâu thuẫn với comment trong CustomMapData ("phải đúng 2").
// Đây là logic NGHIỆP VỤ trên dữ liệu map, không phải trạng thái giao diện —
// tách ra thì kiểm thử được độc lập, và cả Save lẫn Test Play dùng chung một
// nguồn chân lý thay vì mỗi nơi kiểm một kiểu.
//
// Class chỉ có hàm tĩnh, không có state.
// =============================================================================
class CustomMapValidator {
public:
    // Chế độ chơi suy ra TỪ CHÍNH dữ liệu map, không ép cứng ở nơi gọi.
    enum class PlayMode {
        Invalid,        // map không hợp lệ
        SinglePlusBoss, // 1 người chơi đấu boss
        Versus          // 2 người chơi đối kháng
    };

    struct Result {
        bool        valid = false;
        PlayMode    mode  = PlayMode::Invalid;
        std::string message;   // lý do từ chối, hoặc mô tả chế độ khi hợp lệ

        int playerCount() const { return mode == PlayMode::Versus ? 2 : 1; }
        bool isPvP()      const { return mode == PlayMode::Versus; }
    };

    // Kiểm tra map. Không bao giờ ném exception.
    static Result validate(const CustomMapData& data);
};
