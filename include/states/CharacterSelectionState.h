#pragma once
#include "GameState.h"
#include "IrisTransition.h"
#include "Animation.h"
#include <raylib.h>
#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <unordered_map>

// Type alias for the factory function
using LevelFactory = std::function<std::unique_ptr<GameState>(std::string, std::string)>;

struct CharacterCard {
    std::string charName;
    Texture2D cardTex;
    Rectangle hitBox;
    Vector2 position;
    float baseScale;
    float currentScale;
    float targetScale;
    bool isHovered;
    bool isSelected;
    float animOffsetY; // For the intro slide-up animation

    // We store pointer to Animation because Animation doesn't have default constructor
    std::unique_ptr<Animation> idleAnim; 
    std::unique_ptr<Animation> skillAnim; // For random skills on island
};

class CharacterSelectionState : public GameState {
private:
    int numPlayersRequired;
    int currentPlayerSelecting;
    std::string player1Choice;
    std::string player2Choice;
    LevelFactory nextStateFactory;
    std::function<std::unique_ptr<GameState>()> backStateFactory;

    Texture2D backgroundTex;
    Texture2D islandTex;
    Font customFont;

    std::unique_ptr<ITransition> transitionIn;
    std::unique_ptr<ITransition> transitionOut;
    bool isTransitioningIn;
    bool isTransitioningOut;
    bool isTransitioningToPlayer2;

    std::vector<CharacterCard> cards;
    
    // Island preview logic
    Vector2 islandPos;
    float islandScale;
    int hoveredCardIndex; 
    float islandAnimTimer;
    bool islandIsPlayingSkill;
    float islandAnimOffsetY;
    
    // Intro animation
    float introTimer;

    // Back button
    Texture2D backBtnNormal;
    Texture2D backBtnPress;
    Rectangle backBtnRect;
    bool isBackHovered;
    bool isBackPressed;
    bool isBackClicked;
    bool isReturningToMenu;

    // Confirm button
    Texture2D confirmBtnNormal;
    Texture2D confirmBtnPress;
    Rectangle confirmBtnRect;
    bool isConfirmHovered;
    bool isConfirmPressed;
    bool isConfirmClicked;

    void InitCards();
    void UpdateIslandLogic(float dt);

public:
    CharacterSelectionState(int numPlayers, LevelFactory factory, std::function<std::unique_ptr<GameState>()> backStateFactory = nullptr);
    ~CharacterSelectionState() override;

    void HandleInput() override;
    void Process() override;
    void Update(float dt) override;
    void Render(float alpha) const override;
};
