#include "WorldActionPanel.h"
#include <cmath>

// Bảng màu lấy theo tiêu đề của MapSelectionState để hai lớp UI nhìn liền mạch.
static const Color kPanelFill    = {24, 20, 37, 238};
static const Color kPanelBorder  = {248, 34, 0, 255};
static const Color kTitleColor   = {248, 34, 0, 255};
static const Color kBackdropTint = {0, 0, 0, 150};

WorldActionPanel::WorldActionPanel() {}

WorldActionPanel::~WorldActionPanel() {
    if (barTex_.id != 0) UnloadTexture(barTex_);
    if (barPressTex_.id != 0) UnloadTexture(barPressTex_);
    // font_ là tài nguyên MƯỢN -> không Unload ở đây.
}

void WorldActionPanel::Init(float screenWidth, float screenHeight, Font font) {
    screenW_ = screenWidth;
    screenH_ = screenHeight;

    barTex_ = LoadTexture("assets/UI_screens/bar.png");
    barPressTex_ = LoadTexture("assets/UI_screens/bar_press.png");

    font_ = font;
    hasFont_ = (font.texture.id != 0);

    PanelButton* buttons[3] = {&newGameBtn_, &loadGameBtn_, &backBtn_};
    const char* labels[3] = {"NEW GAME", "LOAD GAME", "BACK"};
    for (int i = 0; i < 3; ++i) {
        buttons[i]->SetTextures(barTex_, barPressTex_);
        buttons[i]->SetLabel(labels[i]);
        buttons[i]->SetFontSize(26.0f);
        if (hasFont_) buttons[i]->SetFont(font_);
    }

    RecalculateLayout();
}

void WorldActionPanel::Open(int worldIndex, const std::string& worldName) {
    worldIndex_ = worldIndex;
    worldName_ = worldName;
    isOpen_ = true;
    animT_ = 0.0f;
    newGameBtn_.Reset();
    loadGameBtn_.Reset();
    backBtn_.Reset();
    RecalculateLayout();
}

void WorldActionPanel::Close() {
    isOpen_ = false;
    newGameBtn_.Reset();
    loadGameBtn_.Reset();
    backBtn_.Reset();
}

void WorldActionPanel::RecalculateLayout() {
    const float panelW = 420.0f;
    const float panelH = 320.0f;

    // Độ trượt của animation mở panel được cộng thẳng vào layout (chứ không
    // cộng lúc vẽ) để hitbox của các nút luôn nằm đúng chỗ mắt nhìn thấy —
    // nếu chỉ dời lúc vẽ thì trong 0.22s đầu nút sẽ bấm trượt.
    // Ease-out cubic: bật lên nhanh rồi giảm tốc.
    float ease = 1.0f - powf(1.0f - animT_, 3.0f);
    float slideY = (1.0f - ease) * 40.0f;

    panelRect_ = { (screenW_ - panelW) * 0.5f,
                   (screenH_ - panelH) * 0.5f + slideY,
                   panelW, panelH };

    const float btnW = 280.0f;
    const float btnH = 56.0f;
    const float gap  = 18.0f;
    const float firstY = panelRect_.y + 108.0f;
    const float btnX = panelRect_.x + (panelW - btnW) * 0.5f;

    newGameBtn_.SetBounds({btnX, firstY, btnW, btnH});
    loadGameBtn_.SetBounds({btnX, firstY + (btnH + gap), btnW, btnH});
    backBtn_.SetBounds({btnX, firstY + 2.0f * (btnH + gap), btnW, btnH});
}

void WorldActionPanel::HandleInput(Vector2 mousePos, bool mousePressed, bool mouseReleased) {
    if (!isOpen_) return;

    newGameBtn_.HandleInput(mousePos, mousePressed, mouseReleased);
    loadGameBtn_.HandleInput(mousePos, mousePressed, mouseReleased);
    backBtn_.HandleInput(mousePos, mousePressed, mouseReleased);
}

void WorldActionPanel::Update(float dt) {
    if (!isOpen_) return;

    // Màn hình có thể đổi kích thước giữa chừng -> layout tính lại mỗi frame.
    screenW_ = (float)GetScreenWidth();
    screenH_ = (float)GetScreenHeight();

    if (animT_ < 1.0f) {
        animT_ += dt / 0.22f;
        if (animT_ > 1.0f) animT_ = 1.0f;
    }

    RecalculateLayout();   // sau khi animT_ đã tiến -> hitbox khớp hình vẽ

    newGameBtn_.Update(dt);
    loadGameBtn_.Update(dt);
    backBtn_.Update(dt);

    // Lấy sự kiện click ra ở đây (giai đoạn cập nhật trạng thái), không phải
    // trong HandleInput — giữ đúng phân tách 4 giai đoạn của dự án.
    if (newGameBtn_.ConsumeClick()) {
        if (onNewGame_) onNewGame_();
        return;   // callback có thể đã đóng panel; dừng xử lý nút còn lại
    }
    if (loadGameBtn_.ConsumeClick()) {
        if (onLoadGame_) onLoadGame_();
        return;
    }
    if (backBtn_.ConsumeClick()) {
        Close();
        if (onClose_) onClose_();
        return;
    }

    // ESC cũng đóng panel — thói quen quen thuộc của người chơi PC.
    if (IsKeyPressed(KEY_ESCAPE)) {
        Close();
        if (onClose_) onClose_();
    }
}

void WorldActionPanel::Render() const {
    if (!isOpen_) return;

    // Vị trí đã được RecalculateLayout() tính sẵn trong Update(); ở đây chỉ
    // cần độ mờ. Render() tuyệt đối không đổi state.
    float ease = 1.0f - powf(1.0f - animT_, 3.0f);

    // Nền mờ để tách panel khỏi 6 building phía sau
    DrawRectangle(0, 0, (int)screenW_, (int)screenH_,
                  Color{kBackdropTint.r, kBackdropTint.g, kBackdropTint.b,
                        (unsigned char)(kBackdropTint.a * ease)});

    const Rectangle& r = panelRect_;
    unsigned char alpha = (unsigned char)(255 * ease);
    DrawRectangleRounded(r, 0.08f, 16, Color{kPanelFill.r, kPanelFill.g, kPanelFill.b,
                                             (unsigned char)(kPanelFill.a * ease)});
    DrawRectangleRoundedLines(r, 0.08f, 16, 4.0f,
                              Color{kPanelBorder.r, kPanelBorder.g, kPanelBorder.b, alpha});

    // Tiêu đề "WORLD 0X"
    const float titleSize = 46.0f;
    Color titleCol = {kTitleColor.r, kTitleColor.g, kTitleColor.b, alpha};
    if (hasFont_) {
        Vector2 ts = MeasureTextEx(font_, worldName_.c_str(), titleSize, 2.0f);
        Vector2 tp = {r.x + (r.width - ts.x) * 0.5f, r.y + 34.0f};
        DrawTextEx(font_, worldName_.c_str(), {tp.x + 3.0f, tp.y + 3.0f}, titleSize, 2.0f,
                   Color{0, 0, 0, (unsigned char)(200 * ease)});
        DrawTextEx(font_, worldName_.c_str(), tp, titleSize, 2.0f, titleCol);
    } else {
        int fs = (int)titleSize;
        int tw = MeasureText(worldName_.c_str(), fs);
        DrawText(worldName_.c_str(), (int)(r.x + (r.width - tw) * 0.5f), (int)(r.y + 34.0f), fs, titleCol);
    }

    newGameBtn_.Render();
    loadGameBtn_.Render();
    backBtn_.Render();
}
