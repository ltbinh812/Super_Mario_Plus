#include "EditorCamera.h"
#include <algorithm>
#include <cmath>

EditorCamera::EditorCamera() {
    cam_.target   = {0, 0};
    cam_.offset   = {0, 0};
    cam_.rotation = 0.0f;
    cam_.zoom     = 1.0f;
}

void EditorCamera::reset() {
    cam_.target   = {0, 0};
    cam_.offset   = {0, 0};
    cam_.zoom     = 1.0f;
    isPanning_    = false;
}

void EditorCamera::panTo(float worldX, float worldY) {
    // Đặt camera nhìn vào điểm (worldX, worldY) — đây là world target
    cam_.target = {worldX, worldY};
    // offset sẽ được set đúng trong update()
    cam_.offset = {
        (float)GetScreenWidth()  / 2.0f,
        (float)GetScreenHeight() / 2.0f
    };
}

void EditorCamera::update(float /*dt*/, int mapPixelW, int mapPixelH, bool blockInput) {
    // Luôn đặt offset = center màn hình
    // cam_.target là điểm world mà camera đang "nhìn vào"
    cam_.offset = {
        (float)GetScreenWidth()  / 2.0f,
        (float)GetScreenHeight() / 2.0f
    };

    // --- Zoom (scroll wheel khi KHÔNG giữ chuột giữa) ---
    if (!blockInput) {
        if (!isPanning_) {
            float wheel = GetMouseWheelMove();
            if (wheel != 0.0f) {
                // Zoom hướng về con trỏ chuột
                Vector2 mouseWorld = GetScreenToWorld2D(GetMousePosition(), cam_);
                cam_.zoom += wheel * ZOOM_STEP * cam_.zoom;
                cam_.zoom  = std::clamp(cam_.zoom, ZOOM_MIN, ZOOM_MAX);

                // Giữ nguyên điểm world dưới con trỏ
                Vector2 mouseWorldAfter = GetScreenToWorld2D(GetMousePosition(), cam_);
                cam_.target.x += mouseWorld.x - mouseWorldAfter.x;
                cam_.target.y += mouseWorld.y - mouseWorldAfter.y;
            }
        }

        // --- Pan (giữ chuột giữa + kéo) ---
        if (IsMouseButtonPressed(MOUSE_MIDDLE_BUTTON)) {
            isPanning_       = true;
            panStart_        = GetMousePosition();
            camStartTarget_  = cam_.target;
        }
    }
    
    if (isPanning_) {
        if (IsMouseButtonDown(MOUSE_MIDDLE_BUTTON)) {
            Vector2 delta = {
                (GetMousePosition().x - panStart_.x) / cam_.zoom,
                (GetMousePosition().y - panStart_.y) / cam_.zoom
            };
            // Di chuyển target ngược chiều chuột (camera follows drag)
            cam_.target.x = camStartTarget_.x - delta.x;
            cam_.target.y = camStartTarget_.y - delta.y;
        } else {
            isPanning_ = false;
        }
    }

    // --- Clamp: camera không di chuyển ra ngoài map + MARGIN ---
    // Vùng world mà camera target được phép nằm trong:
    float margin = MARGIN_PX / cam_.zoom;

    // Nửa kích thước màn hình trong không gian world
    float halfW = (float)GetScreenWidth()  / (2.0f * cam_.zoom);
    float halfH = (float)GetScreenHeight() / (2.0f * cam_.zoom);

    float minX = -margin + halfW;
    float maxX = (float)mapPixelW + margin - halfW;
    float minY = -margin + halfH;
    float maxY = (float)mapPixelH + margin - halfH;

    // Đảm bảo min <= max (map quá nhỏ so với màn hình)
    if (minX > maxX) { minX = maxX = (float)mapPixelW / 2.0f; }
    if (minY > maxY) { minY = maxY = (float)mapPixelH / 2.0f; }

    cam_.target.x = std::clamp(cam_.target.x, minX, maxX);
    cam_.target.y = std::clamp(cam_.target.y, minY, maxY);
}

void EditorCamera::beginMode() const {
    BeginMode2D(cam_);
}

void EditorCamera::endMode() const {
    EndMode2D();
}

Vector2 EditorCamera::screenToWorld(Vector2 screenPos) const {
    return GetScreenToWorld2D(screenPos, cam_);
}
