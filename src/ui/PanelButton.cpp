#include "PanelButton.h"

void PanelButton::SetEnabled(bool enabled) {
    enabled_ = enabled;
    if (!enabled_) {
        hovered_ = false;
        pressed_ = false;
        clicked_ = false;
    }
}

void PanelButton::HandleInput(Vector2 mousePos, bool mousePressed, bool mouseReleased) {
    if (!enabled_) return;

    hovered_ = CheckCollisionPointRec(mousePos, bounds_);

    if (mousePressed && hovered_) {
        pressed_ = true;
    }

    if (mouseReleased) {
        // Chỉ tính là click khi bấm xuống VÀ nhả ra đều nằm trong nút — kéo
        // chuột ra ngoài rồi nhả là huỷ, giống hành vi nút chuẩn của hệ điều hành.
        if (hovered_ && pressed_) {
            clicked_ = true;
        }
        pressed_ = false;
    }
}

void PanelButton::Update(float dt) {
    // Nội suy mượt kích thước khi rê chuột. Nhân với dt rồi kẹp về 1.0 để tránh
    // vọt lố khi máy khựng một frame dài.
    float target = (hovered_ && enabled_) ? 1.06f : 1.0f;
    float lerp = 14.0f * dt;
    if (lerp > 1.0f) lerp = 1.0f;
    hoverScale_ += (target - hoverScale_) * lerp;
}

void PanelButton::Render() const {
    // Phóng to quanh tâm nút khi hover
    float w = bounds_.width * hoverScale_;
    float h = bounds_.height * hoverScale_;
    Rectangle dest = {
        bounds_.x + (bounds_.width - w) * 0.5f,
        bounds_.y + (bounds_.height - h) * 0.5f,
        w, h
    };

    Texture2D tex = (pressed_ && texPressed_.id != 0) ? texPressed_ : texNormal_;

    if (tex.id != 0) {
        Rectangle src = {0, 0, (float)tex.width, (float)tex.height};
        Color tint = enabled_ ? WHITE : Color{130, 130, 130, 255};
        DrawTexturePro(tex, src, dest, {0, 0}, 0.0f, tint);
    } else {
        // Không có texture -> vẽ dự phòng để nút vẫn bấm được, không bị "tàng hình"
        Color fill = !enabled_ ? Color{90, 90, 90, 230}
                    : pressed_ ? Color{60, 60, 60, 240}
                    : hovered_ ? Color{235, 235, 235, 240}
                               : Color{200, 200, 200, 235};
        DrawRectangleRounded(dest, 0.35f, 12, fill);
        DrawRectangleRoundedLines(dest, 0.35f, 12, 2.0f, BLACK);
    }

    if (label_.empty()) return;

    Color textColor = enabled_ ? labelColor_ : Color{70, 70, 70, 255};
    if (hasFont_ && font_.texture.id != 0) {
        Vector2 size = MeasureTextEx(font_, label_.c_str(), fontSize_, 1.0f);
        Vector2 pos = { dest.x + (dest.width - size.x) * 0.5f,
                        dest.y + (dest.height - size.y) * 0.5f };
        DrawTextEx(font_, label_.c_str(), pos, fontSize_, 1.0f, textColor);
    } else {
        int fs = (int)fontSize_;
        int tw = MeasureText(label_.c_str(), fs);
        DrawText(label_.c_str(), (int)(dest.x + (dest.width - tw) * 0.5f),
                 (int)(dest.y + (dest.height - fs) * 0.5f), fs, textColor);
    }
}

bool PanelButton::ConsumeClick() {
    if (!clicked_) return false;
    clicked_ = false;
    return true;
}

void PanelButton::Reset() {
    hovered_ = false;
    pressed_ = false;
    clicked_ = false;
    hoverScale_ = 1.0f;
}
