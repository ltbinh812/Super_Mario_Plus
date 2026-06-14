#include "CameraManager.h"

CameraManager::CameraManager() {
    camera_ = { 0 };
    camera_.rotation = 0.0f;
    camera_.zoom = 1.0f;
}

void CameraManager::Init(Player* player) {
    if (player) {
        camera_.target = { player->GetPosition().x + player->GetWidth() * 0.5f,
                           player->GetPosition().y + player->GetHeight() * 0.5f };
    }
    camera_.offset = { GetScreenWidth() * 0.5f, GetScreenHeight() * 0.5f };
}

void CameraManager::Update(Player* player, float worldWidth) {
    if (!player) return;

    float screenW = (float)GetScreenWidth();
    float screenH = (float)GetScreenHeight();

    camera_.offset = { screenW * 0.5f, screenH * 0.5f };

    Vector2 target = {
        player->GetPosition().x + player->GetWidth()  * 0.5f,
        player->GetPosition().y + player->GetHeight() * 0.5f
    };
    camera_.target.x += (target.x - camera_.target.x) * 0.12f;
    camera_.target.y += (target.y - camera_.target.y) * 0.10f;

    float halfW = screenW * 0.5f / camera_.zoom;
    float halfH = screenH * 0.5f / camera_.zoom;
    if (camera_.target.x < halfW)         camera_.target.x = halfW;
    if (camera_.target.x > worldWidth - halfW) camera_.target.x = worldWidth - halfW;
    if (camera_.target.y < halfH)         camera_.target.y = halfH;
}
