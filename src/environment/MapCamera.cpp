#include "MapCamera.h"
#include <iostream>
#include <cmath>
#include <algorithm>

MapCamera::MapCamera(float viewportHeight) 
    : fixedViewportHeight(viewportHeight), zoom(1.0f) {
    camera = { 0 };
    camera.rotation = 0.0f;
    UpdateZoom();
    // Góc camera mặc định của mọi map: để góc trái trên của map (0,0) trùng với góc trái trên của màn hình (0,0)
    float viewW = (float)GetScreenWidth() / (zoom > 0.0f ? zoom : 1.0f);
    float viewH = (float)GetScreenHeight() / (zoom > 0.0f ? zoom : 1.0f);
    camera.target = { viewW / 2.0f, viewH / 2.0f };
}

void MapCamera::UpdateZoom() {
    int screenH = GetScreenHeight();
    if (screenH > 0 && fixedViewportHeight > 0.0f) {
        zoom = (float)screenH / fixedViewportHeight;
    }
    camera.zoom = zoom;
    camera.offset = { (float)GetScreenWidth() / 2.0f, (float)GetScreenHeight() / 2.0f };
}

void MapCamera::Update(Vector2 targetPos, int mapWidth, int mapHeight, float dt) {
    UpdateZoom();

    float viewW = (float)GetScreenWidth() / zoom;
    float viewH = (float)GetScreenHeight() / zoom;

    // Cả 2 map dùng chung chính xác 1 logic camera (bám theo nhân vật trên cả 2 trục X, Y và tự động clamp trong ranh giới map)
    camera.target = targetPos;

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

void MapCamera::UpdateMultiplayer(Vector2 pos1, Vector2 pos2, int mapWidth, int mapHeight, float dt) {
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();
    if (screenH > 0 && fixedViewportHeight > 0.0f && screenW > 0) {
        float baseZoom = (float)screenH / fixedViewportHeight;

        // Tính khoảng cách cần hiển thị giữa 2 nhân vật (kèm lề an toàn padding ~ 320px ngang và 240px dọc)
        float distX = std::abs(pos1.x - pos2.x) + 320.0f;
        float distY = std::abs(pos1.y - pos2.y) + 240.0f;

        float requiredZoomX = (distX > 0.0f) ? ((float)screenW / distX) : baseZoom;
        float requiredZoomY = (distY > 0.0f) ? ((float)screenH / distY) : baseZoom;

        // Zoom không được vượt quá baseZoom và không nhỏ hơn 0.6f (giới hạn zoom out)
        zoom = std::clamp(std::min({ baseZoom, requiredZoomX, requiredZoomY }), 0.6f, baseZoom);
    }
    camera.zoom = zoom;
    camera.offset = { (float)GetScreenWidth() / 2.0f, (float)GetScreenHeight() / 2.0f };

    float viewW = (float)GetScreenWidth() / (zoom > 0.0f ? zoom : 1.0f);
    float viewH = (float)GetScreenHeight() / (zoom > 0.0f ? zoom : 1.0f);

    // Bám theo trung điểm của 2 nhân vật
    camera.target = { (pos1.x + pos2.x) / 2.0f, (pos1.y + pos2.y) / 2.0f };

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
