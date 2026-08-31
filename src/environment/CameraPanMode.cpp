#include "CameraPanMode.h"
#include "MapCamera.h"
#include <cmath>
#include <algorithm>

CameraPanMode::CameraPanMode(Vector2 target, float duration, EaseType easeType,
                             int mapW, int mapH)
    : startPos{0, 0}, targetPos(target), duration(duration), elapsed(0.0f),
      easeType(easeType), started(false), mapWidth(mapW), mapHeight(mapH) {}

float CameraPanMode::ease(float t) const {
    // Clamp t vào [0, 1]
    t = std::clamp(t, 0.0f, 1.0f);

    switch (easeType) {
        case EaseType::Linear:
            return t;

        case EaseType::EaseInOut:
            // Smoothstep: 3t² - 2t³ — chậm ở đầu và cuối, nhanh ở giữa
            return t * t * (3.0f - 2.0f * t);

        case EaseType::EaseOut:
            // Quadratic ease-out: 1 - (1-t)² — nhanh đầu, chậm cuối
            return 1.0f - (1.0f - t) * (1.0f - t);
    }
    return t;
}

void CameraPanMode::update(MapCamera& cam, float dt) {
    // Capture vị trí bắt đầu ở frame đầu tiên
    if (!started) {
        startPos = cam.GetTarget();
        started = true;
    }

    elapsed += dt;
    float t = (duration > 0.0f) ? (elapsed / duration) : 1.0f;
    float easedT = ease(t);

    // Nội suy vị trí camera từ start đến target
    Vector2 newTarget;
    newTarget.x = startPos.x + (targetPos.x - startPos.x) * easedT;
    newTarget.y = startPos.y + (targetPos.y - startPos.y) * easedT;

    // Clamp trong ranh giới map (tương tự logic trong MapCamera::Update)
    float zoom = cam.GetZoom();
    if (zoom > 0.0f) {
        float viewW = (float)GetScreenWidth() / zoom;
        float viewH = (float)GetScreenHeight() / zoom;

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

bool CameraPanMode::isFinished() const {
    return elapsed >= duration;
}

const char* CameraPanMode::getName() const {
    return "CameraPanMode";
}
