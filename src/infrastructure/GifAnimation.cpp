#include "GifAnimation.h"
#include <iostream>
#include <utility>

GifAnimation::~GifAnimation() {
    Unload();
}

GifAnimation::GifAnimation(GifAnimation&& other) noexcept
    : image_(other.image_),
      texture_(other.texture_),
      frameCount_(other.frameCount_),
      currentFrame_(other.currentFrame_),
      frameTime_(other.frameTime_),
      timer_(other.timer_),
      loaded_(other.loaded_) {
    // Bên nguồn phải quên sạch tài nguyên, nếu không destructor của nó sẽ
    // giải phóng đúng texture mà bên đích đang dùng.
    other.image_ = {0};
    other.texture_ = {0};
    other.frameCount_ = 0;
    other.loaded_ = false;
}

GifAnimation& GifAnimation::operator=(GifAnimation&& other) noexcept {
    if (this == &other) return *this;
    Unload();

    image_        = other.image_;
    texture_      = other.texture_;
    frameCount_   = other.frameCount_;
    currentFrame_ = other.currentFrame_;
    frameTime_    = other.frameTime_;
    timer_        = other.timer_;
    loaded_       = other.loaded_;

    other.image_ = {0};
    other.texture_ = {0};
    other.frameCount_ = 0;
    other.loaded_ = false;
    return *this;
}

bool GifAnimation::Load(const std::string& filePath, float fps) {
    Unload();

    if (!FileExists(filePath.c_str())) {
        std::cerr << "[GifAnimation] Khong tim thay file: " << filePath << "\n";
        return false;
    }

    int frames = 0;
    image_ = LoadImageAnim(filePath.c_str(), &frames);
    if (image_.data == nullptr || frames <= 0) {
        std::cerr << "[GifAnimation] Khong giai ma duoc: " << filePath << "\n";
        if (image_.data != nullptr) UnloadImage(image_);
        image_ = {0};
        return false;
    }

    texture_ = LoadTextureFromImage(image_);
    if (texture_.id == 0) {
        std::cerr << "[GifAnimation] Khong tao duoc texture tu: " << filePath << "\n";
        UnloadImage(image_);
        image_ = {0};
        return false;
    }

    frameCount_   = frames;
    currentFrame_ = 0;
    timer_        = 0.0f;
    frameTime_    = (fps > 0.0f) ? (1.0f / fps) : 0.05f;
    loaded_       = true;

    std::cout << "[GifAnimation] " << filePath << " — " << frames << " frame, "
              << texture_.width << "x" << texture_.height << "\n";
    return true;
}

void GifAnimation::Unload() {
    if (texture_.id != 0) {
        UnloadTexture(texture_);
        texture_ = {0};
    }
    if (image_.data != nullptr) {
        UnloadImage(image_);
        image_ = {0};
    }
    frameCount_ = 0;
    currentFrame_ = 0;
    timer_ = 0.0f;
    loaded_ = false;
}

void GifAnimation::Update(float dt) {
    // Ảnh tĩnh (1 frame) thì không có gì để chạy.
    if (!loaded_ || frameCount_ <= 1) return;

    timer_ += dt;
    if (timer_ < frameTime_) return;

    // Dùng while thay vì if: nếu một frame game bị khựng lâu hơn frameTime_,
    // hoạt ảnh vẫn bắt kịp thời gian thực thay vì bị trôi chậm dần.
    while (timer_ >= frameTime_) {
        timer_ -= frameTime_;
        currentFrame_ = (currentFrame_ + 1) % frameCount_;
    }

    // Mỗi frame chiếm width * height * 4 byte (định dạng R8G8B8A8) và nằm nối
    // tiếp nhau trong image_.data — đây chính là quy ước của LoadImageAnim.
    unsigned int offset =
        (unsigned int)image_.width * (unsigned int)image_.height * 4u * (unsigned int)currentFrame_;
    UpdateTexture(texture_, (const unsigned char*)image_.data + offset);
}

void GifAnimation::DrawFullscreen(Color tint) const {
    if (!loaded_) return;
    Rectangle src  = {0.0f, 0.0f, (float)texture_.width, (float)texture_.height};
    Rectangle dest = {0.0f, 0.0f, (float)GetScreenWidth(), (float)GetScreenHeight()};
    DrawTexturePro(texture_, src, dest, {0.0f, 0.0f}, 0.0f, tint);
}

void GifAnimation::DrawPro(Rectangle dest, Vector2 origin, Color tint) const {
    if (!loaded_) return;
    Rectangle src = {0.0f, 0.0f, (float)texture_.width, (float)texture_.height};
    DrawTexturePro(texture_, src, dest, origin, 0.0f, tint);
}
