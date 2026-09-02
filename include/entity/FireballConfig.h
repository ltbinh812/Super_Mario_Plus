#pragma once
#include <string>

struct FireballConfig {
    float speed          = 300.0f;
    float gravityScale   = 0.0f;    // 0 = straight, >0 = arcing down
    float lifetime       = 2.0f;
    int   damage         = 10;
    float hitboxW        = 16.0f;
    float hitboxH        = 16.0f;
    float hitboxOffsetX  = 0.0f;    // hitbox offset from center (flips with facing)
    float hitboxOffsetY  = 0.0f;    // hitbox offset from bottom
    float offsetX        = 100.0f;  // horizontal spawn offset from player (flips with facing)
    float offsetY        = -10.0f;  // vertical spawn offset from player
    float curveAmplitude = 0.0f;    // 0 = straight, >0 = sine wave
    float curveFrequency = 0.0f;
    std::string textureName = "";   // empty = draw circle fallback
    std::string soundKey    = "";   // sound to stop when destroyed
    int   frameNum       = 1;
    float frameTime      = 0.1f;
    float scale          = 1.0f;

    // --- Hai tuỳ chọn CÁCH VẼ, bật riêng cho từng nhân vật trong JSON --------

    // beamFromOwner: nối một TIA SÁNG từ người bắn tới quả đạn, và tia đó dài
    // dần ra khi đạn bay xa.
    //
    // Dùng cho chưởng kamehameha của Goku. Tia sáng vốn được vẽ SẴN trong chính
    // sprite nhân vật (special_attack.png, khung 5-8), nhưng nó bị cắt cụt ở mép
    // khung 128px — nên trên màn hình tia luôn dài đúng ~82px rồi hết, trong khi
    // quả cầu vẫn bay tiếp. Bật cờ này thì cột điểm ảnh ngoài cùng của khung
    // (đúng phần đuôi tia) được kéo giãn để nối tiếp tới chỗ quả cầu.
    //
    // Quả cầu KHÔNG bị đụng tới: nó vẫn vẽ nguyên kích thước như cũ.
    bool  beamFromOwner = false;

    // alignFramesByContent: canh MỖI KHUNG theo tâm phần ảnh không trong suốt
    // của chính nó (cả trục X lẫn trục Y), thay vì canh theo mép khung.
    //
    // Cần cho phi tiêu của Naruto. Đo trên vfx_1.png (khung 256x256):
    //     khung 0-8  (phi tiêu)  tâm X = 0.115   tâm Y = 0.88
    //     khung 9-12 (quả cầu nổ) tâm X = 0.46   tâm Y = 0.65
    // Canh theo mép khung thì lúc phi tiêu nổ thành quả cầu, hình nhảy ~88px
    // theo trục X và ~58px theo trục Y — điểm cuối phi tiêu và điểm đầu quả cầu
    // không trùng nhau.
    bool  alignFramesByContent = false;
};
