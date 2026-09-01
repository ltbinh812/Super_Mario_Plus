#pragma once
#include "raylib.h"
#include <string>

// =============================================================================
// GifAnimation — Bọc một file GIF động thành một Texture2D tự chạy hoạt ảnh.
//
// VÌ SAO CẦN CLASS NÀY?
// `LoadTexture("x.gif")` của raylib CHỈ nạp khung hình đầu tiên -> ảnh đứng im.
// Muốn chạy được phải dùng cặp API khác:
//     Image img = LoadImageAnim(path, &frames);   // nạp TẤT CẢ frame vào img.data
//     Texture2D tex = LoadTextureFromImage(img);  // frame đầu lên GPU
//     ...mỗi lần đổi frame:
//     UpdateTexture(tex, (unsigned char*)img.data + width*height*4*frameIndex);
//
// Đoạn này lặt vặt và dễ sai (quên nhân 4 byte/pixel, quên giữ img sống, quên
// UnloadImage). Gói lại một chỗ để cả UI (CharacterSelectionState) lẫn entity
// (EndgameAsset) dùng chung, thay vì chép hai lần.
//
// LƯU Ý BỘ NHỚ: `image_` phải nằm trong RAM suốt vòng đời vì mỗi lần đổi frame
// ta đọc lại từ nó. Dung lượng = width * height * 4 * frameCount byte.
// Ví dụ GIF 1400x787 có 14 frame chiếm ~62MB RAM. Muốn nhẹ hơn thì giảm kích
// thước hoặc số frame của chính file GIF.
//
// SỞ HỮU TÀI NGUYÊN: class này SỞ HỮU cả Image (RAM) lẫn Texture (VRAM) nên
// cấm sao chép; chỉ cho phép di chuyển (move) để tránh giải phóng hai lần.
//
// 4 GIAI ĐOẠN: Update(dt) tiến khung hình (đổi trạng thái), Draw*() chỉ vẽ.
// Tuyệt đối KHÔNG gọi Update() bên trong hàm render của state/entity.
// =============================================================================
class GifAnimation {
private:
    Image     image_   = {0};   // toàn bộ frame nối tiếp nhau trong .data
    Texture2D texture_ = {0};   // frame đang hiển thị trên GPU
    int   frameCount_   = 0;
    int   currentFrame_ = 0;
    float frameTime_    = 0.05f; // giây mỗi frame
    float timer_        = 0.0f;
    bool  loaded_       = false;

public:
    GifAnimation() = default;
    ~GifAnimation();

    // Cấm sao chép: hai bản sao sẽ cùng giải phóng một texture -> hỏng.
    GifAnimation(const GifAnimation&) = delete;
    GifAnimation& operator=(const GifAnimation&) = delete;

    // Cho phép di chuyển để còn cất vào vector/unique_ptr được.
    GifAnimation(GifAnimation&& other) noexcept;
    GifAnimation& operator=(GifAnimation&& other) noexcept;

    // Nạp file. `fps` là tốc độ phát mong muốn — raylib KHÔNG đọc được độ trễ
    // ghi trong file GIF nên phải truyền tay (vd: GIF 100ms/frame -> fps = 10).
    // Trả false nếu file không tồn tại hoặc không giải mã được.
    bool Load(const std::string& filePath, float fps = 20.0f);
    void Unload();

    void Update(float dt);

    bool IsLoaded() const { return loaded_; }
    int  GetFrameCount() const { return frameCount_; }
    Texture2D GetTexture() const { return texture_; }
    float GetWidth() const { return (float)texture_.width; }
    float GetHeight() const { return (float)texture_.height; }

    // Kéo giãn phủ kín toàn màn hình (dùng cho ảnh nền).
    void DrawFullscreen(Color tint = WHITE) const;

    // Vẽ vào một hình chữ nhật đích với điểm neo tuỳ ý (dùng cho entity trong map).
    void DrawPro(Rectangle dest, Vector2 origin, Color tint = WHITE) const;
};
