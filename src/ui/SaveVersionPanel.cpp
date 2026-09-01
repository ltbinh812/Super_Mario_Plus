#include "SaveVersionPanel.h"
#include <cmath>

static const Color kPanelFill    = {24, 20, 37, 240};
static const Color kPanelBorder  = {248, 34, 0, 255};
static const Color kTitleColor   = {248, 34, 0, 255};
static const Color kBackdropTint = {0, 0, 0, 160};
static const Color kRowFill      = {48, 42, 70, 220};
static const Color kRowFillHover = {72, 64, 100, 235};
static const Color kRowFillSel   = {120, 60, 30, 240};
static const Color kRowText      = {235, 230, 220, 255};

// Chiều cao một dòng và khoảng cách giữa các dòng — dùng ở cả layout lẫn vẽ.
static const float kRowH   = 44.0f;
static const float kRowGap = 8.0f;

SaveVersionPanel::SaveVersionPanel() {}

SaveVersionPanel::~SaveVersionPanel() {
    if (barTex_.id != 0) UnloadTexture(barTex_);
    if (barPressTex_.id != 0) UnloadTexture(barPressTex_);
    // font_ là tài nguyên MƯỢN -> không Unload.
}

void SaveVersionPanel::Init(float screenWidth, float screenHeight, Font font) {
    screenW_ = screenWidth;
    screenH_ = screenHeight;

    barTex_ = LoadTexture("assets/UI_screens/bar.png");
    barPressTex_ = LoadTexture("assets/UI_screens/bar_press.png");

    font_ = font;
    hasFont_ = (font.texture.id != 0);

    PanelButton* buttons[3] = {&deleteBtn_, &loadBtn_, &backBtn_};
    const char* labels[3] = {"DELETE", "LOAD", "BACK"};
    for (int i = 0; i < 3; ++i) {
        buttons[i]->SetTextures(barTex_, barPressTex_);
        buttons[i]->SetLabel(labels[i]);
        buttons[i]->SetFontSize(24.0f);
        if (hasFont_) buttons[i]->SetFont(font_);
    }

    RecalculateLayout();
}

void SaveVersionPanel::Open(int worldIndex, const std::string& worldName,
                            std::vector<SaveSlotInfo> slots) {
    worldIndex_ = worldIndex;
    title_ = "LOAD GAME - " + worldName;
    slots_ = std::move(slots);
    selectedIndex_ = -1;
    scrollY_ = 0.0f;
    isOpen_ = true;
    animT_ = 0.0f;
    pendingRowClick_ = false;
    pendingRowIndex_ = -1;
    pendingWheel_ = 0.0f;
    deleteBtn_.Reset();
    loadBtn_.Reset();
    backBtn_.Reset();
    RecalculateLayout();
}

void SaveVersionPanel::Refresh(std::vector<SaveSlotInfo> slots) {
    slots_ = std::move(slots);
    // Sau khi xoá, chỉ số cũ không còn ý nghĩa -> bỏ chọn, ẩn lại DELETE/LOAD.
    selectedIndex_ = -1;
    if (scrollY_ > 0.0f) scrollY_ = 0.0f;
    RecalculateLayout();
}

void SaveVersionPanel::Close() {
    isOpen_ = false;
    selectedIndex_ = -1;
    deleteBtn_.Reset();
    loadBtn_.Reset();
    backBtn_.Reset();
}

void SaveVersionPanel::RecalculateLayout() {
    const float panelW = 720.0f;
    const float panelH = 460.0f;

    float ease = 1.0f - powf(1.0f - animT_, 3.0f);
    float slideY = (1.0f - ease) * 40.0f;

    panelRect_ = { (screenW_ - panelW) * 0.5f,
                   (screenH_ - panelH) * 0.5f + slideY,
                   panelW, panelH };

    // Vùng danh sách: chừa chỗ cho tiêu đề ở trên và hàng nút ở dưới.
    const float listTop = panelRect_.y + 82.0f;
    const float listBottom = panelRect_.y + panelH - 92.0f;
    listRect_ = { panelRect_.x + 28.0f, listTop, panelW - 56.0f, listBottom - listTop };

    // Tổng chiều cao nội dung so với vùng hiển thị -> giới hạn cuộn.
    float contentH = slots_.empty() ? 0.0f
                                    : (float)slots_.size() * kRowH + (slots_.size() - 1) * kRowGap;
    maxScrollY_ = contentH - listRect_.height;
    if (maxScrollY_ < 0.0f) maxScrollY_ = 0.0f;
    if (scrollY_ > maxScrollY_) scrollY_ = maxScrollY_;
    if (scrollY_ < 0.0f) scrollY_ = 0.0f;

    // Hàng nút dưới cùng. DELETE + LOAD chỉ hiện khi đã chọn một dòng, nên khi
    // chưa chọn ta căn giữa mỗi nút BACK cho cân đối.
    const float btnW = 180.0f;
    const float btnH = 52.0f;
    const float btnY = panelRect_.y + panelH - 72.0f;
    const float gap = 20.0f;

    if (selectedIndex_ >= 0) {
        // Thứ tự trái -> phải: DELETE | BACK | LOAD.
        // BACK nằm giữa để hai hành động "nặng" (xoá vĩnh viễn và vào game)
        // tách xa nhau, giảm nguy cơ bấm nhầm DELETE khi định bấm LOAD.
        float totalW = btnW * 3.0f + gap * 2.0f;
        float startX = panelRect_.x + (panelW - totalW) * 0.5f;
        deleteBtn_.SetBounds({startX, btnY, btnW, btnH});
        backBtn_.SetBounds({startX + btnW + gap, btnY, btnW, btnH});
        loadBtn_.SetBounds({startX + 2.0f * (btnW + gap), btnY, btnW, btnH});
    } else {
        backBtn_.SetBounds({panelRect_.x + (panelW - btnW) * 0.5f, btnY, btnW, btnH});
    }
}

Rectangle SaveVersionPanel::RowRect(int index) const {
    return { listRect_.x,
             listRect_.y + index * (kRowH + kRowGap) - scrollY_,
             listRect_.width,
             kRowH };
}

std::string SaveVersionPanel::RowText(const SaveSlotInfo& slot) const {
    const SaveMetaData& m = slot.meta;

    int totalSec = (int)m.playTimeSeconds;
    int mins = totalSec / 60;
    int secs = totalSec % 60;

    std::string levelName = m.levelId.empty() ? std::string("-") : m.levelId;
    // Cắt bớt tên level quá dài để không tràn khỏi dòng (LDtk có level tên
    // dài như "Your_typical_2D_platformer").
    if (levelName.size() > 18) levelName = levelName.substr(0, 17) + ".";

    std::string charName = m.characterName.empty() ? std::string("?") : m.characterName;

    return "v" + std::to_string(slot.versionIndex)
         + "   " + charName
         + "   " + levelName
         + "   " + std::to_string(m.coins) + "c"
         + "   " + std::to_string(m.health) + "/" + std::to_string(m.maxHealth)
         + "   " + std::to_string(mins) + "m" + (secs < 10 ? "0" : "") + std::to_string(secs) + "s";
}

void SaveVersionPanel::HandleInput(Vector2 mousePos, bool mousePressed, bool mouseReleased) {
    if (!isOpen_) return;

    mousePos_ = mousePos;

    deleteBtn_.HandleInput(mousePos, mousePressed, mouseReleased);
    loadBtn_.HandleInput(mousePos, mousePressed, mouseReleased);
    backBtn_.HandleInput(mousePos, mousePressed, mouseReleased);

    // Chỉ GHI NHẬN thao tác, việc đổi selectedIndex_/scrollY_ để Update() làm.
    pendingWheel_ += GetMouseWheelMove();

    if (mouseReleased) {
        pendingRowIndex_ = -1;
        // Click phải nằm trong vùng danh sách, nếu không thì phần dòng bị cắt
        // ra ngoài scissor vẫn ăn click — bấm hụt rất khó chịu.
        if (CheckCollisionPointRec(mousePos, listRect_)) {
            for (int i = 0; i < (int)slots_.size(); ++i) {
                if (CheckCollisionPointRec(mousePos, RowRect(i))) {
                    pendingRowIndex_ = i;
                    pendingRowClick_ = true;
                    break;
                }
            }
        }
    }
}

void SaveVersionPanel::Update(float dt) {
    if (!isOpen_) return;

    screenW_ = (float)GetScreenWidth();
    screenH_ = (float)GetScreenHeight();

    if (animT_ < 1.0f) {
        animT_ += dt / 0.22f;
        if (animT_ > 1.0f) animT_ = 1.0f;
    }

    // Cuộn danh sách
    if (pendingWheel_ != 0.0f) {
        scrollY_ -= pendingWheel_ * 48.0f;
        pendingWheel_ = 0.0f;
    }

    // Chọn / bỏ chọn một dòng
    if (pendingRowClick_) {
        pendingRowClick_ = false;
        if (pendingRowIndex_ >= 0 && pendingRowIndex_ < (int)slots_.size()) {
            // Bấm lại đúng dòng đang chọn thì bỏ chọn -> hai nút ẩn đi.
            selectedIndex_ = (selectedIndex_ == pendingRowIndex_) ? -1 : pendingRowIndex_;
        }
        pendingRowIndex_ = -1;
    }

    RecalculateLayout();   // sau khi scroll/selection đã đổi -> hitbox khớp hình

    // Hai nút này chỉ tồn tại khi có dòng được chọn.
    bool hasSelection = (selectedIndex_ >= 0 && selectedIndex_ < (int)slots_.size());
    deleteBtn_.SetEnabled(hasSelection);
    loadBtn_.SetEnabled(hasSelection);

    deleteBtn_.Update(dt);
    loadBtn_.Update(dt);
    backBtn_.Update(dt);

    if (hasSelection) {
        // Sao chép slot ra biến cục bộ TRƯỚC khi gọi callback: onDelete_ sẽ
        // Refresh() lại slots_, làm tham chiếu vào phần tử cũ thành treo.
        SaveSlotInfo slot = slots_[selectedIndex_];

        if (loadBtn_.ConsumeClick()) {
            if (onLoad_) onLoad_(slot);
            return;
        }
        if (deleteBtn_.ConsumeClick()) {
            if (onDelete_) onDelete_(slot);
            return;
        }
    }

    if (backBtn_.ConsumeClick() || IsKeyPressed(KEY_ESCAPE)) {
        Close();
        if (onClose_) onClose_();
    }
}

void SaveVersionPanel::Render() const {
    if (!isOpen_) return;

    float ease = 1.0f - powf(1.0f - animT_, 3.0f);
    unsigned char alpha = (unsigned char)(255 * ease);

    DrawRectangle(0, 0, (int)screenW_, (int)screenH_,
                  Color{kBackdropTint.r, kBackdropTint.g, kBackdropTint.b,
                        (unsigned char)(kBackdropTint.a * ease)});

    const Rectangle& r = panelRect_;
    DrawRectangleRounded(r, 0.06f, 16, Color{kPanelFill.r, kPanelFill.g, kPanelFill.b,
                                             (unsigned char)(kPanelFill.a * ease)});
    DrawRectangleRoundedLines(r, 0.06f, 16, 4.0f,
                              Color{kPanelBorder.r, kPanelBorder.g, kPanelBorder.b, alpha});

    // --- Tiêu đề ---
    const float titleSize = 38.0f;
    Color titleCol = {kTitleColor.r, kTitleColor.g, kTitleColor.b, alpha};
    if (hasFont_) {
        Vector2 ts = MeasureTextEx(font_, title_.c_str(), titleSize, 2.0f);
        Vector2 tp = {r.x + (r.width - ts.x) * 0.5f, r.y + 26.0f};
        DrawTextEx(font_, title_.c_str(), {tp.x + 3.0f, tp.y + 3.0f}, titleSize, 2.0f,
                   Color{0, 0, 0, (unsigned char)(200 * ease)});
        DrawTextEx(font_, title_.c_str(), tp, titleSize, 2.0f, titleCol);
    } else {
        int fs = (int)titleSize;
        int tw = MeasureText(title_.c_str(), fs);
        DrawText(title_.c_str(), (int)(r.x + (r.width - tw) * 0.5f), (int)(r.y + 26.0f), fs, titleCol);
    }

    // --- Danh sách bản lưu ---
    if (slots_.empty()) {
        const char* msg = "NO SAVED GAME";
        const float ms = 32.0f;
        if (hasFont_) {
            Vector2 s = MeasureTextEx(font_, msg, ms, 2.0f);
            DrawTextEx(font_, msg,
                       {listRect_.x + (listRect_.width - s.x) * 0.5f,
                        listRect_.y + (listRect_.height - s.y) * 0.5f},
                       ms, 2.0f, Color{170, 165, 160, alpha});
        } else {
            int tw = MeasureText(msg, (int)ms);
            DrawText(msg, (int)(listRect_.x + (listRect_.width - tw) * 0.5f),
                     (int)(listRect_.y + listRect_.height * 0.5f), (int)ms,
                     Color{170, 165, 160, alpha});
        }
    } else {
        // Cắt theo vùng danh sách để dòng cuộn ra ngoài không đè lên tiêu đề/nút.
        BeginScissorMode((int)listRect_.x, (int)listRect_.y,
                         (int)listRect_.width, (int)listRect_.height);

        for (int i = 0; i < (int)slots_.size(); ++i) {
            Rectangle row = RowRect(i);
            // Bỏ qua dòng nằm hoàn toàn ngoài vùng nhìn thấy
            if (row.y + row.height < listRect_.y || row.y > listRect_.y + listRect_.height) continue;

            bool isSelected = (i == selectedIndex_);
            bool isHovered = CheckCollisionPointRec(mousePos_, row) &&
                             CheckCollisionPointRec(mousePos_, listRect_);

            Color fill = isSelected ? kRowFillSel : (isHovered ? kRowFillHover : kRowFill);
            fill.a = (unsigned char)(fill.a * ease);
            DrawRectangleRounded(row, 0.25f, 8, fill);
            if (isSelected) {
                DrawRectangleRoundedLines(row, 0.25f, 8, 3.0f,
                                          Color{kPanelBorder.r, kPanelBorder.g, kPanelBorder.b, alpha});
            }

            std::string text = RowText(slots_[i]);
            const float rowFont = 22.0f;
            Color textCol = {kRowText.r, kRowText.g, kRowText.b, alpha};
            if (hasFont_) {
                Vector2 s = MeasureTextEx(font_, text.c_str(), rowFont, 1.0f);
                DrawTextEx(font_, text.c_str(),
                           {row.x + 18.0f, row.y + (row.height - s.y) * 0.5f},
                           rowFont, 1.0f, textCol);
            } else {
                int fs = (int)rowFont;
                DrawText(text.c_str(), (int)(row.x + 18.0f),
                         (int)(row.y + (row.height - fs) * 0.5f), fs, textCol);
            }
        }

        EndScissorMode();

        // Thanh cuộn mảnh bên phải, chỉ hiện khi danh sách dài hơn khung.
        if (maxScrollY_ > 0.0f) {
            float trackX = listRect_.x + listRect_.width + 8.0f;
            DrawRectangleRounded({trackX, listRect_.y, 6.0f, listRect_.height}, 1.0f, 6,
                                 Color{60, 55, 80, alpha});
            float ratio = listRect_.height / (listRect_.height + maxScrollY_);
            float thumbH = listRect_.height * ratio;
            float thumbY = listRect_.y + (scrollY_ / maxScrollY_) * (listRect_.height - thumbH);
            DrawRectangleRounded({trackX, thumbY, 6.0f, thumbH}, 1.0f, 6,
                                 Color{kPanelBorder.r, kPanelBorder.g, kPanelBorder.b, alpha});
        }
    }

    // --- Hàng nút: DELETE/LOAD chỉ vẽ khi đã chọn một bản lưu ---
    if (selectedIndex_ >= 0) {
        deleteBtn_.Render();
        loadBtn_.Render();
    }
    backBtn_.Render();
}
