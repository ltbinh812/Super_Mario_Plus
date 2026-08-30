#pragma once
#include "GameState.h"
#include "IrisTransition.h"
#include <raylib.h>
#include <memory>
#include <vector>

struct MapNode {
    Texture2D tex;
    Vector2 position;
    float baseScale;
    float currentScale;
    float targetScale;
    Rectangle hitBox;
    int worldIndex;
    bool isHovered;
    bool isClicked;
};

enum class MapSelectionMode {
    SinglePlayer,
    PvP
};

class MapSelectionState : public GameState {
private:
    Texture2D backgroundTex;
    Font customFont;
    std::unique_ptr<ITransition> transitionIn;
    std::unique_ptr<ITransition> transitionOut;
    bool isTransitioningIn;
    bool isTransitioningOut;
    
    std::vector<MapNode> mapNodes;
    int targetWorldIndex;
    MapSelectionMode currentMode;

    // Back button
    Texture2D backBtnNormal;
    Texture2D backBtnPress;
    Rectangle backBtnRect;
    bool isBackHovered;
    bool isBackPressed;
    bool isBackClicked;
    bool isReturningToMenu;

    void InitNodes();

public:
    MapSelectionState(MapSelectionMode mode = MapSelectionMode::SinglePlayer);
    ~MapSelectionState() override;

    void HandleInput() override;
    void Process() override;
    void Update(float dt) override;
    void Render(float alpha) const override;
};
