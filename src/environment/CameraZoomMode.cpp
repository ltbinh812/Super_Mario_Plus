#include "CameraZoomMode.h"
#include "MapCamera.h"
#include <cmath>
#include <algorithm>

// Constructor chỉ zoom (không pan)
CameraZoomMode::CameraZoomMode(float targetZoom, float duration, EaseType easeType, int mapW, int mapH)
    : startZoom(1.0f), targetZoom(targetZoom),
      panStart{0, 0}, panTarget{0, 0}, hasPan(false),
      duration(duration), elapsed(0.0f),
      easeType(easeType), started(false), mapWidth(mapW), mapHeight(mapH) {}

// Constructor zoom + pan đồng thời
CameraZoomMode::CameraZoomMode(float targetZoom, Vector2 panTarget, float duration,
                               EaseType easeType, int mapW, int mapH)
    : startZoom(1.0f), targetZoom(targetZoom),
      panStart{0, 0}, panTarget(panTarget), hasPan(true),
      duration(duration), elapsed(0.0f),
      easeType(easeType), started(false), mapWidth(mapW), mapHeight(mapH) {}

float CameraZoomMode::ease(float t) const {
    t = std::clamp(t, 0.0f, 1.0f);
    switch (easeType) {
        case EaseType::Linear:
            return t;
        case EaseType::EaseInOut:
            return t * t * (3.0f - 2.0f * t);
        case EaseType::EaseOut:
            return 1.0f - (1.0f - t) * (1.0f - t);
    }
    return t;
}

void CameraZoomMode::update(MapCamera& cam, float dt) {
    // Capture trạng thái ban đầu ở frame đầu tiên
    if (!started) {
        startZoom = cam.GetZoom();
        panStart = cam.GetTarget();
        started = true;
    }

    elapsed += dt;
    float t = (duration > 0.0f) ? (elapsed / duration) : 1.0f;
    float easedT = ease(t);

    // Nội suy zoom
    float newZoom = startZoom + (targetZoom - startZoom) * easedT;
    cam.setCameraZoom(newZoom);

    // Tính toán target mới (dù có pan hay không, target vẫn cần clamp theo zoom mới)
    Vector2 newTarget = hasPan ? panStart : cam.GetTarget();
    if (hasPan) {
        newTarget.x = panStart.x + (panTarget.x - panStart.x) * easedT;
        newTarget.y = panStart.y + (panTarget.y - panStart.y) * easedT;
    }

    // Luôn luôn clamp target theo zoom mới (giống CameraPanMode)
    if (newZoom > 0.0f) {
        float viewW = (float)GetScreenWidth() / newZoom;
        float viewH = (float)GetScreenHeight() / newZoom;

        if (mapWidth > 0) {
            float minX = viewW / 2.0f;
            float maxX = std::max(minX, (float)mapWidth - viewW / 2.0f);
            newTarget.x = std::clamp(newTarget.x, minX, maxX);
        }
        if (mapHeight > 0) {
            float minY = viewH / 2.0f;
            float maxY = std::max(minY, (float)mapHeight - viewH / 2.0f);
            newTarget.y = std::clamp(newTarget.y, minY, maxY);
        }
    }
    cam.setCameraTarget(newTarget);
}

bool CameraZoomMode::isFinished() const {
    return elapsed >= duration;
}

const char* CameraZoomMode::getName() const {
    return "CameraZoomMode";
}
