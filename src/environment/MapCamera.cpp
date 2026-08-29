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

void MapCamera::Update(Vector2 targetPos, Vector2 playerVelocity, int mapWidth, int mapHeight, float dt) {
    // 1. Dynamic Zooming
    int screenH = GetScreenHeight();
    if (screenH > 0 && fixedViewportHeight > 0.0f) {
        float baseZoom = (float)screenH / fixedViewportHeight;
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

    if (targetPos.x < leftEdge) logicalTarget.x -= (leftEdge - targetPos.x);
    else if (targetPos.x > rightEdge) logicalTarget.x += (targetPos.x - rightEdge);

    if (targetPos.y < topEdge) logicalTarget.y -= (topEdge - targetPos.y);
    else if (targetPos.y > bottomEdge) logicalTarget.y += (targetPos.y - bottomEdge);

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
    }

    if (mapHeight > 0) {
        float minY = viewH / 2.0f;
        float maxY = std::max(minY, (float)mapHeight - viewH / 2.0f);
        camera.target.y = std::clamp(camera.target.y, minY, maxY);
    }
}


void MapCamera::MoveManual(Vector2 delta) {
    camera.target.x += delta.x;
    camera.target.y += delta.y;
}

void MapCamera::BeginMode() const {
    BeginMode2D(camera);
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
