#pragma once

// =============================================================================
// Công tắc vẽ hình debug.
//
// Trước đây các khung hitbox được gọi vẽ thẳng, không qua điều kiện nào, nên
// bản chạy cho người khác xem lúc nào cũng đầy khung đỏ quanh nhân vật, quái,
// đạn và vùng cutscene.
//
// Để là `constexpr` chứ không phải biến chạy: trình biên dịch bỏ hẳn thân lệnh
// khi cờ tắt, nên bản giao nộp không tốn một phép vẽ nào. Đổi thành `true` rồi
// build lại là có đủ khung để chỉnh timing hitbox.
//
// KHÔNG gộp vào SettingsManager: đây là công cụ của người làm game, không phải
// tuỳ chọn của người chơi, và không có lý do gì để nó đi vào file save.
// =============================================================================
namespace DebugDraw {

// Khung hitbox của người chơi, quái, boss, đạn, vụ nổ, và vùng CutsceneTrigger.
inline constexpr bool kShowHitboxes = false;

}  // namespace DebugDraw
