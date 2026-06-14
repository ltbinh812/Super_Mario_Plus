#ifndef WORLD_H
#define WORLD_H

#include "GameState.h"
#include "Player.h"
#include "Platform.h"
#include "Cloud.h"
#include "Decoration.h"
#include "CameraManager.h"
#include "OverlayUI.h"
#include <vector>
#include <string>
#include <memory>

class World1_1State : public GameState {
public:
    World1_1State();
    ~World1_1State() override = default;

    void HandleInput() override;
    void Update(float dt) override;
    void Render(float alpha) const override;

private:
    
    float worldWidth_;
    float groundY_;

    bool isSelectingCharacter_ = false; // Bật
    bool isSettingsOpen_ = false; // Bật
    int player1CharIndex_ = 0; // Bật
    int player2CharIndex_ = 1; // Bật
    std::vector<std::string> availableCharacters_ = {"Mario", "Luigi", "Peach", "Toad", "Wario"};

    std::unique_ptr<OverlayUI> activeOverlay_;

    std::unique_ptr<Player> player1_;
    std::unique_ptr<Player> player2_;
    CameraManager camera_;
    std::vector<std::unique_ptr<Character>> enemies_;
    std::vector<Platform> platforms_;
    std::vector<Cloud> clouds_;
    std::vector<Decoration> decorations_;
};

#endif
