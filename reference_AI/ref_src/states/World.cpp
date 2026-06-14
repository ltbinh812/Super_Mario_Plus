#include "World.h"
#include "Background.h"
#include "HUD.h"
#include "Menu.h"
#include "raylib.h"
#include <cstdlib>
#include <memory>
#include "CharacterFactory.h"
#include "HighJumpAbility.h"
#include "DashAbility.h"
#include "SettingsOverlay.h"
#include "CharacterSelectionOverlay.h"

World1_1State::World1_1State() {
    worldWidth_ = 6000.0f;
    groundY_ = 720 - 80.0f;

    player1_ = std::unique_ptr<Player>(static_cast<Player*>(CharacterFactory::GetInstance().CreateCharacter(availableCharacters_[player1CharIndex_]).release()));
    player1_->Init(200.0f, groundY_ - player1_->GetHeight());
    player1_->SetInputConfig({ KEY_A, KEY_D, KEY_W, KEY_S, KEY_J, KEY_K });

    player2_ = std::unique_ptr<Player>(static_cast<Player*>(CharacterFactory::GetInstance().CreateCharacter(availableCharacters_[player2CharIndex_]).release()));
    player2_->Init(250.0f, groundY_ - player2_->GetHeight());
    player2_->SetInputConfig({ KEY_LEFT, KEY_RIGHT, KEY_UP, KEY_DOWN, KEY_KP_1, KEY_KP_2 });

    enemies_.clear();
    auto goomba1 = CharacterFactory::GetInstance().CreateCharacter("Goomba");
    goomba1->Init(600.0f, groundY_ - 32.0f);
    enemies_.push_back(std::move(goomba1));

    auto goomba2 = CharacterFactory::GetInstance().CreateCharacter("Goomba");
    goomba2->Init(900.0f, groundY_ - 32.0f);
    enemies_.push_back(std::move(goomba2));

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

    camera_.Init(player1_.get());
}

void World1_1State::HandleInput() {
    if (activeOverlay_) {
        activeOverlay_->HandleInput();
        if (activeOverlay_->IsFinished()) {
            activeOverlay_.reset();
        }
        return;
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        activeOverlay_ = std::make_unique<SettingsOverlay>([this](Command&& cmd) {
            this->PushCommand(std::move(cmd));
        });
        return;
    }

    if (IsKeyPressed(KEY_U)) {
        activeOverlay_ = std::make_unique<CharacterSelectionOverlay>(player1_, player2_, availableCharacters_);
        return;
    }
    
    // Test kỹ năng đặc biệt cho Player 1
    if (IsKeyPressed(KEY_J)) player1_->UseAbility1();
    if (IsKeyPressed(KEY_K)) player1_->UseAbility2();
    
    // Test kỹ năng đặc biệt cho Player 2
    if (IsKeyPressed(KEY_KP_1)) player2_->UseAbility1();
    if (IsKeyPressed(KEY_KP_2)) player2_->UseAbility2();
}

void World1_1State::Update(float dt) {
    if (activeOverlay_) {
        activeOverlay_->Update(dt);
        return; // Tạm dừng update
    }

    player1_->Update(dt, worldWidth_);
    player1_->SetOnGround(false);
    for (auto& p : platforms_) player1_->ResolveCollision(p.GetBounds());

    player2_->Update(dt, worldWidth_);
    player2_->SetOnGround(false);
    for (auto& p : platforms_) player2_->ResolveCollision(p.GetBounds());

    for (auto& e : enemies_) {
        e->Update(dt, worldWidth_);
        e->SetOnGround(false);
        for (auto& p : platforms_) e->ResolveCollision(p.GetBounds());
    }

    camera_.Update(player1_.get(), worldWidth_);
    for (auto& c : clouds_) c.Update(dt, worldWidth_);
}

void World1_1State::Render(float alpha) const {
    ClearBackground({ 107, 186, 226, 255 });

    BeginMode2D(camera_.GetCamera());
        Background::Draw(worldWidth_, groundY_, clouds_);
        for (const auto& d : decorations_) d.Draw();
        for (const auto& p : platforms_) p.Draw();
        player1_->Draw();
        player2_->Draw();
        for (auto const& e : enemies_) e->Draw(); // Changed auto& to auto const& for const correctness
    EndMode2D();

    HUD::Draw(*player1_);

    if (activeOverlay_) {
        activeOverlay_->Render(alpha);
    }
}
