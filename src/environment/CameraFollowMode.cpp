#include "CameraFollowMode.h"
#include "MapCamera.h"

CameraFollowMode::CameraFollowMode()
    : targetPos1{0, 0}, targetVel{0, 0}, mapWidth(0), mapHeight(0) {}

void CameraFollowMode::setTarget(Vector2 pos, Vector2 vel, int mapW, int mapH) {
    targetPos1 = pos;
    targetVel = vel;
    mapWidth = mapW;
    mapHeight = mapH;
}

void CameraFollowMode::update(MapCamera& cam, float dt) {
    // Delegate sang logic đã có sẵn trong MapCamera
    // MapCamera::Update đã xử lý zoom, clamp, offset
    cam.Update(targetPos1, targetVel, mapWidth, mapHeight, dt);
}

bool CameraFollowMode::isFinished() const {
    // Mode Follow không bao giờ "xong" — chạy vĩnh viễn cho đến khi bị thay thế
    return false;
}

const char* CameraFollowMode::getName() const {
    return "CameraFollowMode";
}
