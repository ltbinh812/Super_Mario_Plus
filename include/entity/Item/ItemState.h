#pragma once

// =============================================================================
// ItemState — Vòng đời của một item trên map (State Pattern, dạng enum nhẹ).
//
// Tách riêng khỏi BaseItem.h để tầng Save (include/save/LevelSaveData.h) có thể
// dùng enum này mà KHÔNG phải kéo theo Entity.h / raylib.h / ItemAtlasRegistry.h.
//
//   Idle   : trạng thái ban đầu khi vừa spawn từ LDtk (rương chưa mở, cờ chưa cắm)
//   Active : đã bị kích hoạt nhưng vẫn còn tồn tại trên map (cờ đã cắm, cửa đã mở)
//   Used   : đã tiêu thụ xong, không còn render/va chạm (coin đã ăn, buff đã nhặt)
//
// Giá trị số được ghi thẳng xuống file save (0/1/2) — KHÔNG đổi thứ tự các phần
// tử, nếu không mọi file save cũ sẽ hiểu sai trạng thái item.
// =============================================================================
enum class ItemState { Idle, Active, Used };
