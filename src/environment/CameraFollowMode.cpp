#include "CameraFollowMode.h"
#include "MapCamera.h"

CameraFollowMode::CameraFollowMode()
    : targetPos1{0, 0}, targetPos2{0, 0},
      mapWidth(0), mapHeight(0), isMultiplayer(false) {}

void CameraFollowMode::setTarget(Vector2 pos, int mapW, int mapH) {
    targetPos1 = pos;
    mapWidth = mapW;
    mapHeight = mapH;
    isMultiplayer = false;
}

void CameraFollowMode::setTargetMultiplayer(Vector2 pos1, Vector2 pos2, int mapW, int mapH) {
    targetPos1 = pos1;
    targetPos2 = pos2;
    mapWidth = mapW;
    mapHeight = mapH;
    isMultiplayer = true;
}

void CameraFollowMode::update(MapCamera& cam, float dt) {
    // Delegate sang logic đã có sẵn trong MapCamera
    // MapCamera::Update / UpdateMultiplayer đã xử lý zoom, clamp, offset
    if (isMultiplayer) {
        cam.UpdateMultiplayer(targetPos1, targetPos2, mapWidth, mapHeight, dt);
    } else {
        cam.Update(targetPos1, mapWidth, mapHeight, dt);
    }
}

bool CameraFollowMode::isFinished() const {
    // Mode Follow không bao giờ "xong" — chạy vĩnh viễn cho đến khi bị thay thế
    return false;
}

const char* CameraFollowMode::getName() const {
    return "CameraFollowMode";
}
