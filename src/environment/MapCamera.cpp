#include "MapCamera.h"
#include <iostream>
#include <cmath>
#include <algorithm>

MapCamera::MapCamera(float viewportHeight) 
    : fixedViewportHeight(viewportHeight), zoom(1.0f),
      deadzoneSize{100.0f, 150.0f}, logicalTarget{0.0f, 0.0f},
      followSpeedX(4.0f), followSpeedY(6.0f),
      lookAheadDistance(120.0f), lookAheadSpeed(3.0f),
      currentLookAheadX(0.0f), targetZoom(0.0f), zoomSpeed(2.0f) {
    camera = { 0 };
    camera.rotation = 0.0f;
    UpdateZoom();
    // Góc camera mặc định của mọi map: để góc trái trên của map (0,0) trùng với góc trái trên của màn hình (0,0)
    float viewW = (float)GetScreenWidth() / (zoom > 0.0f ? zoom : 1.0f);
    float viewH = (float)GetScreenHeight() / (zoom > 0.0f ? zoom : 1.0f);
    camera.target = { viewW / 2.0f, viewH / 2.0f };
    logicalTarget = camera.target;
}

void MapCamera::UpdateZoom() {
    int screenH = GetScreenHeight();
    if (screenH > 0 && fixedViewportHeight > 0.0f) {
        targetZoom = (float)screenH / fixedViewportHeight;
        zoom = targetZoom;
    }
    camera.zoom = zoom;
    camera.offset = { (float)GetScreenWidth() / 2.0f, (float)GetScreenHeight() / 2.0f };
}

void MapCamera::Update(Vector2 targetPos, Vector2 playerVelocity, int mapWidth, int mapHeight, float dt, const Vector2* targetPos2) {
    // 1. Calculate actual target and dynamic zooming
    Vector2 actualTarget = targetPos;
    int screenH = GetScreenHeight();
    float baseZoom = (screenH > 0 && fixedViewportHeight > 0.0f) ? (float)screenH / fixedViewportHeight : 1.0f;

    if (targetPos2 != nullptr) {
        // Calculate midpoint
        actualTarget.x = (targetPos.x + targetPos2->x) / 2.0f;
        actualTarget.y = (targetPos.y + targetPos2->y) / 2.0f;
        
        // Calculate dynamic zoom based on distance
        float distanceX = std::abs(targetPos.x - targetPos2->x);
        float distanceY = std::abs(targetPos.y - targetPos2->y);
        
        float screenW = (float)GetScreenWidth();
        
        // Padding from edges
        float paddingX = screenW * 0.3f; // 15% on each side
        float paddingY = screenH * 0.3f; 
        
        float requiredZoomX = (screenW - paddingX) / (distanceX > 1.0f ? distanceX : 1.0f);
        float requiredZoomY = (screenH - paddingY) / (distanceY > 1.0f ? distanceY : 1.0f);
        
        float requiredZoom = std::min(requiredZoomX, requiredZoomY);
        
        // Cho phép zoom out vô hạn (không giới hạn minZoom), nhưng không bao giờ zoom in vượt quá baseZoom
        targetZoom = std::min(requiredZoom, baseZoom);
    } else {
        if (targetZoom == 0.0f) targetZoom = baseZoom; // Default
    }
    
    // Smooth zoom interpolation
    if (dt > 0.0f) {
        zoom += (targetZoom - zoom) * zoomSpeed * dt;
    }
    camera.zoom = zoom;
    camera.offset = { (float)GetScreenWidth() / 2.0f, (float)GetScreenHeight() / 2.0f };

    // 2. Deadzone calculation
    float leftEdge = logicalTarget.x - deadzoneSize.x / 2.0f;
    float rightEdge = logicalTarget.x + deadzoneSize.x / 2.0f;
    float topEdge = logicalTarget.y - deadzoneSize.y / 2.0f;
    float bottomEdge = logicalTarget.y + deadzoneSize.y / 2.0f;

    if (actualTarget.x < leftEdge) logicalTarget.x -= (leftEdge - actualTarget.x);
    else if (actualTarget.x > rightEdge) logicalTarget.x += (actualTarget.x - rightEdge);

    if (actualTarget.y < topEdge) logicalTarget.y -= (topEdge - actualTarget.y);
    else if (actualTarget.y > bottomEdge) logicalTarget.y += (actualTarget.y - bottomEdge);

    // 3. Target Look-ahead
    float targetLookAheadX = 0.0f;
    if (playerVelocity.x > 10.0f) targetLookAheadX = lookAheadDistance;
    else if (playerVelocity.x < -10.0f) targetLookAheadX = -lookAheadDistance;
    
    if (dt > 0.0f) {
        currentLookAheadX += (targetLookAheadX - currentLookAheadX) * lookAheadSpeed * dt;
    } else {
        currentLookAheadX = targetLookAheadX;
    }

    // 4. Smooth Follow (Lerp)
    Vector2 idealPosition = { logicalTarget.x + currentLookAheadX, logicalTarget.y };
    
    if (dt > 0.0f) {
        camera.target.x += (idealPosition.x - camera.target.x) * followSpeedX * dt;
        camera.target.y += (idealPosition.y - camera.target.y) * followSpeedY * dt;
    } else {
        camera.target = idealPosition;
    }

    // 5. Clamp to map boundaries
    float viewW = (float)GetScreenWidth() / (zoom > 0.0f ? zoom : 1.0f);
    float viewH = (float)GetScreenHeight() / (zoom > 0.0f ? zoom : 1.0f);

    if (mapWidth > 0) {
        float minX = viewW / 2.0f;
        float maxX = std::max(minX, (float)mapWidth - viewW / 2.0f);
        camera.target.x = std::clamp(camera.target.x, minX, maxX);
        logicalTarget.x = std::clamp(logicalTarget.x, minX, maxX);
    }

    if (mapHeight > 0) {
        float minY = viewH / 2.0f;
        float maxY = std::max(minY, (float)mapHeight - viewH / 2.0f);
        camera.target.y = std::clamp(camera.target.y, minY, maxY);
        logicalTarget.y = std::clamp(logicalTarget.y, minY, maxY);
    }
}


void MapCamera::MoveManual(Vector2 delta) {
    camera.target.x += delta.x;
    camera.target.y += delta.y;
}

// =============================================================================
// Rung màn hình.
//
// shake() lấy MẠNH HƠN giữa cú đang rung và cú mới, thay vì ghi đè. Nếu ghi đè
// thì một đòn nhẹ ngay sau đòn nặng sẽ làm cú rung lớn tắt ngóm giữa chừng.
// =============================================================================
void MapCamera::shake(float intensity, float duration) {
    if (intensity <= 0.0f || duration <= 0.0f) return;
    if (intensity >= shakeIntensity_ || shakeTimer_ <= 0.0f) {
        shakeIntensity_ = intensity;
        shakeDuration_  = duration;
        shakeTimer_     = duration;
    }
}

void MapCamera::updateShake(float dt) {
    if (shakeTimer_ <= 0.0f) {
        shakeOffset_ = {0.0f, 0.0f};
        return;
    }
    shakeTimer_ -= dt;
    if (shakeTimer_ <= 0.0f) {
        shakeTimer_ = 0.0f;
        shakeIntensity_ = 0.0f;
        shakeOffset_ = {0.0f, 0.0f};
        return;
    }
    // Biên độ tắt dần tuyến tính -> cú rung "nảy mạnh rồi lặng", không cắt cụt.
    const float falloff = shakeTimer_ / shakeDuration_;
    const float amp = shakeIntensity_ * falloff;
    shakeOffset_.x = ((float)GetRandomValue(-100, 100) / 100.0f) * amp;
    shakeOffset_.y = ((float)GetRandomValue(-100, 100) / 100.0f) * amp;
}

void MapCamera::BeginMode() const {
    // Áp độ lệch rung lên BẢN SAO, không đụng vào camera.target thật.
    // Nếu cộng thẳng vào target thì logic bám người chơi sẽ lấy vị trí đã bị
    // rung làm điểm xuất phát cho frame sau, và camera trôi dần khỏi nhân vật.
    Camera2D shaken = camera;
    shaken.target.x += shakeOffset_.x;
    shaken.target.y += shakeOffset_.y;
    BeginMode2D(shaken);
}

void MapCamera::EndMode() const {
    EndMode2D();
}

// === Camera Mode System Implementation ===

void MapCamera::pushMode(std::unique_ptr<ICameraMode> mode) {
    if (!mode) return;

    if (!currentMode) {
        // Không có mode nào đang chạy → bắt đầu ngay
        currentMode = std::move(mode);
        isInCinematicMode = true;
    } else {
        // Đã có mode đang chạy → xếp hàng chờ
        modeQueue.push(std::move(mode));
    }
}

void MapCamera::clearModes() {
    currentMode.reset();
    // Clear toàn bộ queue
    std::queue<std::unique_ptr<ICameraMode>> empty;
    std::swap(modeQueue, empty);
    isInCinematicMode = false;
}

void MapCamera::updateMode(float dt) {
    if (!currentMode) return;

    currentMode->update(*this, dt);

    if (currentMode->isFinished()) {
        advanceMode();
    }
}

void MapCamera::advanceMode() {
    // Mode hiện tại đã hoàn thành → chuyển sang mode tiếp theo trong queue
    if (!modeQueue.empty()) {
        currentMode = std::move(modeQueue.front());
        modeQueue.pop();
    } else {
        // Hết mode → thoát cinematic
        currentMode.reset();
        isInCinematicMode = false;
    }
}
