#pragma once

// Các loại tool khả dụng trong editor.
// Tách ra file riêng theo quy tắc 1 khai báo / 1 file của dự án (trước đây nằm
// chung trong EditorToolManager.h), và để các panel UI dùng được mà không phải
// kéo theo cả manager.
enum class EditorToolType { Place, PlaceEntity, Erase };
