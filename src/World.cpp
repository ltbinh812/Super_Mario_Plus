#include "World.h"
#include "Background.h"
#include "HUD.h"
#include "GameManager.h"
#include "Menu.h"
#include "raylib.h"
#include <cstdlib>
#include <memory>

World1_1State::World1_1State(GameManager* gameManager) : gameManager_(gameManager) {
    worldWidth_ = 6000.0f;
    groundY_ = 720 - 80.0f;

    player_.Init(200.0f, groundY_ - player_.GetHeight());

    platforms_.clear();
    platforms_.push_back({ { 0, groundY_, worldWidth_, 80.0f }, { 94, 164, 50, 255 } });
    platforms_.push_back({ { 800,  groundY_ - 64,  64,  64 }, { 139, 90, 43, 255 } });
    platforms_.push_back({ { 864,  groundY_ - 128, 64,  128}, { 139, 90, 43, 255 } });
    platforms_.push_back({ { 928,  groundY_ - 192, 64,  192}, { 139, 90, 43, 255 } });
    platforms_.push_back({ { 1200, groundY_ - 160, 192, 32 }, { 139, 90, 43, 255 } });
    platforms_.push_back({ { 1500, groundY_ - 240, 192, 32 }, { 139, 90, 43, 255 } });
    platforms_.push_back({ { 1800, groundY_ - 180, 256, 32 }, { 139, 90, 43, 255 } });
    platforms_.push_back({ { 2200, groundY_, worldWidth_ - 2200, 80.0f }, { 94, 164, 50, 255 } });
    platforms_.push_back({ { 2400, groundY_ - 200, 192, 32 }, { 139, 90, 43, 255 } });
    platforms_.push_back({ { 2700, groundY_ - 140, 256, 32 }, { 139, 90, 43, 255 } });
    platforms_.push_back({ { 3000, groundY_ - 200, 192, 32 }, { 139, 90, 43, 255 } });
    platforms_.push_back({ { 3300, groundY_ - 280, 128, 32 }, { 139, 90, 43, 255 } });

    clouds_.clear();
    float cloudPositions[][2] = {
        { 200, 100 }, { 600, 60 }, { 1000, 130 }, { 1400, 80 },
        { 1900, 110 }, { 2400, 70 }, { 2900, 140 }, { 3400, 90 },
        { 3900, 120 }, { 4400, 60 }, { 4900, 100 }
    };
    for (auto& cp : cloudPositions) {
        clouds_.push_back(Cloud({ cp[0], cp[1] }, 15.0f + (float)(std::rand() % 20), 0.8f + (float)(std::rand() % 6) * 0.1f));
    }

    decorations_.clear();
    float decoData[][3] = {
        { 120,  groundY_, 0 }, { 350,  groundY_, 1 }, { 500,  groundY_, 0 },
        { 1100, groundY_, 2 }, { 1450, groundY_, 0 }, { 1700, groundY_, 1 },
        { 2300, groundY_, 0 }, { 2600, groundY_, 2 }, { 2800, groundY_, 1 },
        { 3100, groundY_, 0 }, { 3500, groundY_, 2 }, { 3800, groundY_, 1 },
        { 4100, groundY_, 0 }, { 4500, groundY_, 1 }, { 4900, groundY_, 2 },
    };
    for (auto& dd : decoData) {
        decorations_.push_back(Decoration({ dd[0], dd[1] }, (int)dd[2]));
    }

    camera_.Init(&player_);
}

void World1_1State::HandleInput() {
    // Nhấn phím M để thoát ra Menu chính (minh hoạ state transition)
    if (IsKeyPressed(KEY_M)) {
        gameManager_->ChangeState(std::make_unique<MenuState>(gameManager_));
    }
}

void World1_1State::Update(float dt) {
    player_.ResetGroundState();
    player_.Update(dt, worldWidth_);
    for (auto& p : platforms_) player_.ResolveCollision(p.GetBounds());
    camera_.Update(&player_, worldWidth_);
    for (auto& c : clouds_) c.Update(dt, worldWidth_);
}

void World1_1State::Draw() {
    ClearBackground({ 107, 186, 226, 255 });

    BeginMode2D(camera_.GetCamera());
        Background::Draw(worldWidth_, groundY_, clouds_);
        for (const auto& d : decorations_) d.Draw();
        for (const auto& p : platforms_) p.Draw();
        player_.Draw();
    EndMode2D();

    HUD::Draw(player_);
}
