#ifndef CAMERAMANAGER_H
#define CAMERAMANAGER_H

#include "raylib.h"
#include "Player.h"

class CameraManager {
public:
    CameraManager();
    void Init(Player* player);
    void Update(Player* player, float worldWidth);
    Camera2D GetCamera() const { return camera_; }

private:
    Camera2D camera_;
};

#endif
