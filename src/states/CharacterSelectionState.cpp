#include "CharacterSelectionState.h"
#include "StateCommands.h"
#include "LoadingState.h"
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include "AssetManager.h"

using json = nlohmann::json;

CharacterSelectionState::CharacterSelectionState(int numPlayers, LevelFactory factory)
    : numPlayersRequired(numPlayers), currentPlayerSelecting(1), nextStateFactory(std::move(factory)) 
{
    isTransitioningIn = true;
    isTransitioningOut = false;
    transitionIn = std::make_unique<IrisTransition>(true, 1.0f);
    transitionOut = std::make_unique<IrisTransition>(false, 1.0f);

    backgroundTex = LoadTexture("assets/UI_screens/map_selection.png"); // Re-use background
    islandTex = LoadTexture("assets/UI_screens/flying_island.png");
    customFont = LoadFont("assets/config/kenney-pixel-hu.otf");

    backBtnNormal = LoadTexture("assets/UI_screens/menu_btn_back.png");
    backBtnPress = LoadTexture("assets/UI_screens/menu_btn_back_press.png");
    confirmBtnNormal = LoadTexture("assets/UI_screens/bar.png");
    confirmBtnPress = LoadTexture("assets/UI_screens/bar_press.png");
    isBackHovered = false;
    isBackPressed = false;
    isBackClicked = false;
    isReturningToMenu = false;

    isConfirmHovered = false;
    isConfirmPressed = false;
    isConfirmClicked = false;
    
    // Bottom right confirm button
    float btnW = 200.0f;
    float btnH = 60.0f;
    confirmBtnRect = { (float)GetScreenWidth() - btnW - 30.0f, (float)GetScreenHeight() - btnH - 30.0f, btnW, btnH };

    hoveredCardIndex = -1;
    islandAnimTimer = 0.0f;
    islandIsPlayingSkill = false;
    islandScale = (float)GetScreenHeight() / (float)islandTex.width * 0.60;
    islandPos = { (float)GetScreenWidth() * 0.25f, (float)GetScreenHeight() * 0.50f };

    InitCards();
}

CharacterSelectionState::~CharacterSelectionState() {
    UnloadTexture(backgroundTex);
    UnloadTexture(islandTex);
    UnloadFont(customFont);
    UnloadTexture(backBtnNormal);
    UnloadTexture(backBtnPress);
    UnloadTexture(confirmBtnNormal);
    UnloadTexture(confirmBtnPress);
}

void CharacterSelectionState::InitCards() {
    std::vector<std::string> charNames = {"Goku", "Naruto", "Luffy", "Kakashi", "Sasuke", "Zoro"};
    std::ifstream file("assets/config/characters.json");
    if (!file.is_open()) {
        std::cerr << "Cannot open characters.json" << std::endl;
        return;
    }
    json jsonData;
    file >> jsonData;

    float screenW = GetScreenWidth();
    float screenH = GetScreenHeight();
    
    // Spread 6 cards nicely
    Vector2 positions[6] = {
        {screenW * 0.75f, screenH * 0.35f},
        {screenW * 0.50f, screenH * 0.35f},
        {screenW * 0.75f, screenH * 0.35f},
        {screenW * 0.25f, screenH * 0.65f},
        {screenW * 0.50f, screenH * 0.65f},
        {screenW * 0.75f, screenH * 0.65f}
    };

    int i = 0;
    Texture2D cardBgTex = LoadTexture("assets/UI_screens/character_card.png");
    // Ensure we unload it if we load it per card, actually let's just store it in cardTex for each card
    // since they all share it, it's fine. Wait, if they all share it, we shouldn't unload it 6 times.
    // I'll just load it directly.

    for (const auto& name : charNames) {
        if (!jsonData.contains(name)) {
            i++;
            continue;
        }
        auto& charData = jsonData[name];
        std::string assetFolder = charData["assetFolder"].get<std::string>();
        
        CharacterCard card;
        card.charName = name;
        card.cardTex = cardBgTex;
        card.position = positions[i];
        card.baseScale = 0.5f;
        card.currentScale = card.baseScale;
        card.targetScale = card.baseScale;
        card.isHovered = false;
        card.isSelected = false;
        
        float w = card.cardTex.width * card.baseScale;
        float h = card.cardTex.height * card.baseScale;
        card.hitBox = {card.position.x - w/2.0f, card.position.y - h/2.0f, w, h};
        
        // Load Idle Animation
        if (charData["animations"].contains("idle")) {
            auto& animData = charData["animations"]["idle"];
            std::string texBase = animData["texture"].get<std::string>();
            std::string texKey = name + "_" + texBase;
            std::string texPath = "assets/" + assetFolder + "/" + texBase + ".png";
            AssetManager::getInstance().loadTexture(texKey, texPath);
            int frames = animData["frameNum"].get<int>();
            float time = animData["frameTime"].get<float>();
            float scale = animData.value("scale", 1.0f);
            card.idleAnim = std::make_unique<Animation>(AssetManager::getInstance().getTexture(texKey), frames, time, scale);
        }

        // Load Skill Animation
        if (charData["animations"].contains("attack_1")) {
            auto& animData = charData["animations"]["attack_1"];
            std::string texBase = animData["texture"].get<std::string>();
            std::string texKey = name + "_skill_" + texBase;
            std::string texPath = "assets/" + assetFolder + "/" + texBase + ".png";
            AssetManager::getInstance().loadTexture(texKey, texPath);
            int frames = animData["frameNum"].get<int>();
            float time = animData["frameTime"].get<float>();
            float scale = animData.value("scale", 1.0f);
            card.skillAnim = std::make_unique<Animation>(AssetManager::getInstance().getTexture(texKey), frames, time, scale);
            card.skillAnim->setLoop(false);
        }

        cards.push_back(std::move(card));
        i++;
    }
}

void CharacterSelectionState::HandleInput() {
    if (isTransitioningIn || isTransitioningOut) return;

    Vector2 mousePos = GetMousePosition();
    bool mousePressed = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    bool mouseReleased = IsMouseButtonReleased(MOUSE_LEFT_BUTTON);

    // Back button logic
    float baseSize = 48.0f;
    float hoverSize = 56.0f;
    Rectangle hitBox = { 10.0f, 10.0f, hoverSize, hoverSize };
    isBackHovered = CheckCollisionPointRec(mousePos, hitBox);
    
    if (backBtnNormal.id != 0) {
        float btnSize = isBackHovered ? hoverSize : baseSize;
        float offset = (hoverSize - btnSize) / 2.0f;
        backBtnRect = { 10.0f + offset, 10.0f + offset, btnSize, btnSize };
        if (isBackHovered && mousePressed) isBackPressed = true;
    }
    
    if (mouseReleased) {
        if (isBackHovered && isBackPressed) {
            isBackClicked = true;
        }
        isBackPressed = false;
    }

    // Check if any card is selected to enable confirm button
    bool hasSelection = false;
    for (const auto& card : cards) {
        if (card.isSelected) {
            hasSelection = true;
            break;
        }
    }

    // Confirm button logic
    isConfirmHovered = hasSelection && CheckCollisionPointRec(mousePos, confirmBtnRect);
    if (isConfirmHovered && mousePressed) isConfirmPressed = true;
    if (mouseReleased) {
        if (isConfirmHovered && isConfirmPressed) {
            isConfirmClicked = true;
        }
        isConfirmPressed = false;
    }

    // Card hover and click
    hoveredCardIndex = -1;
    for (size_t i = 0; i < cards.size(); i++) {
        cards[i].isHovered = CheckCollisionPointRec(mousePos, cards[i].hitBox);
        if (cards[i].isHovered) {
            hoveredCardIndex = i;
            if (mouseReleased) {
                // Deselect all others
                for (auto& c : cards) c.isSelected = false;
                cards[i].isSelected = true;
            }
        }
    }
}

void CharacterSelectionState::Process() {
    if (isTransitioningIn || isTransitioningOut) return;

    if (isBackClicked) {
        isBackClicked = false;
        // The MapSelectionState will be returned to since this state was pushed.
        // Wait, if it was pushed, we can pop. If it was changed, we need a ChangeStateCommand.
        // We will just change state back to MapSelectionState. Wait, the exact previous mode is lost if we don't pass it!
        // The user didn't request a BACK button, but I'll add a simple PopStateCommand for it.
        // Actually, if we use Pop, it returns to the MapSelectionState exactly as it was.
        isReturningToMenu = true;
        isTransitioningOut = true;
        transitionOut->Start(true);
        return;
    }

    if (isConfirmClicked) {
        isConfirmClicked = false;
        for (size_t i = 0; i < cards.size(); i++) {
            if (cards[i].isSelected) {
                if (currentPlayerSelecting == 1) {
                    player1Choice = cards[i].charName;
                    if (numPlayersRequired == 1) {
                        isTransitioningOut = true;
                        transitionOut->Start(true);
                    } else {
                        currentPlayerSelecting = 2; // Move to Player 2
                        for (auto& c : cards) c.isSelected = false; // Reset selection
                    }
                } else if (currentPlayerSelecting == 2) {
                    player2Choice = cards[i].charName;
                    isTransitioningOut = true;
                    transitionOut->Start(true);
                }
                break;
            }
        }
    }
}

void CharacterSelectionState::UpdateIslandLogic(float dt) {
    int selectedIndex = -1;
    for (size_t i = 0; i < cards.size(); i++) {
        if (cards[i].isSelected) {
            selectedIndex = (int)i;
            break;
        }
    }

    if (selectedIndex != -1 && selectedIndex < cards.size()) {
        auto& card = cards[selectedIndex];
        
        if (!islandIsPlayingSkill) {
            // Random chance to play skill (e.g. 50% chance every 3s)
            islandAnimTimer += dt;
            if (islandAnimTimer > 3.0f) {
                if (GetRandomValue(0, 100) < 50 && card.skillAnim) { 
                    islandIsPlayingSkill = true;
                    card.skillAnim->resetAnimation();
                }
                islandAnimTimer = 0.0f;
            }
        } else {
            if (card.skillAnim) {
                card.skillAnim->update(dt);
                if (card.skillAnim->isFinished()) {
                    islandIsPlayingSkill = false;
                }
            } else {
                islandIsPlayingSkill = false;
            }
        }
    } else {
        islandAnimTimer = 0.0f;
        islandIsPlayingSkill = false;
    }
}

void CharacterSelectionState::Update(float dt) {
    if (isTransitioningIn) {
        transitionIn->Update(dt);
        if (transitionIn->IsFinished()) isTransitioningIn = false;
    }

    if (isTransitioningOut) {
        transitionOut->Update(dt);
        if (transitionOut->IsFinished()) {
            if (isReturningToMenu) {
                this->PushStateCommand(std::make_unique<::PopStateCommand>());
                return;
            }
            this->PushStateCommand(std::make_unique<::ChangeStateCommand>(
                std::make_unique<LoadingState>([this]() { return nextStateFactory(player1Choice, player2Choice); }, 1.0f)
            ));
            return;
        }
    }

    // Update cards animations and logic
    for (auto& card : cards) {
        if (card.isHovered || card.isSelected) {
            card.targetScale = card.baseScale * 1.15f;
        } else {
            card.targetScale = card.baseScale;
        }
        float lerpFactor = 12.0f * dt;
        if (lerpFactor > 1.0f) lerpFactor = 1.0f;
        card.currentScale += (card.targetScale - card.currentScale) * lerpFactor;

        float w = card.cardTex.width * card.currentScale;
        float h = card.cardTex.height * card.currentScale;
        card.hitBox = {card.position.x - w/2.0f, card.position.y - h/2.0f, w, h};
        
        if (card.idleAnim) {
            card.idleAnim->update(dt);
        }
    }

    UpdateIslandLogic(dt);
}

void CharacterSelectionState::Render(float alpha) const {
    ClearBackground(BLACK);

    if (backgroundTex.id != 0) {
        Rectangle src = { 0, 0, (float)backgroundTex.width, (float)backgroundTex.height };
        Rectangle dest = { 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() };
        DrawTexturePro(backgroundTex, src, dest, {0, 0}, 0.0f, WHITE);
    }

    // Draw Title
    std::string titleStr = (currentPlayerSelecting == 1) ? "PLAYER 1: SELECT CHARACTER" : "PLAYER 2: SELECT CHARACTER";
    float fontSize = 70.0f;
    Vector2 textSize = MeasureTextEx(customFont, titleStr.c_str(), fontSize, 2.0f);
    Vector2 titlePos = { (GetScreenWidth() - textSize.x) / 2.0f, 30.0f };
    DrawTextEx(customFont, titleStr.c_str(), {titlePos.x + 4.0f, titlePos.y + 4.0f}, fontSize, 2.0f, {0, 0, 0, 200});
    DrawTextEx(customFont, titleStr.c_str(), titlePos, fontSize, 2.0f, WHITE);

    // Draw Island
    if (islandTex.id != 0) {
        Rectangle src = { 0, 0, (float)islandTex.width, (float)islandTex.height };
        float islandW = islandTex.width * islandScale;
        float islandH = islandTex.height * islandScale;
        Vector2 origin = { islandW / 2.0f, islandH / 2.0f };
        DrawTexturePro(islandTex, src, {islandPos.x, islandPos.y, islandW, islandH}, origin, 0.0f, WHITE);
    }

    // Draw Selected Character on Island
    int selectedIndex = -1;
    for (size_t i = 0; i < cards.size(); i++) {
        if (cards[i].isSelected) {
            selectedIndex = (int)i;
            break;
        }
    }

    if (selectedIndex != -1 && selectedIndex < cards.size()) {
        const auto& card = cards[selectedIndex];
        Animation* currentAnim = (islandIsPlayingSkill && card.skillAnim) ? card.skillAnim.get() : card.idleAnim.get();
        
        if (currentAnim) {
            Rectangle src = currentAnim->getCurrentFrame();
            // Flip x to face left maybe? Or just keep it as is
            float scale = currentAnim->getScale() * 2.0f * islandScale; // Make it bigger on island and scale it with island
            Rectangle dest = { islandPos.x, islandPos.y - (40.0f * islandScale), src.width * scale, src.height * scale };
            Vector2 origin = { dest.width / 2.0f, dest.height / 2.0f };
            DrawTexturePro(currentAnim->getTexture(), src, dest, origin, 0.0f, WHITE);
        }
    }

    // Draw Cards
    for (const auto& card : cards) {
        if (card.cardTex.id != 0) {
            Rectangle src = { 0, 0, (float)card.cardTex.width, (float)card.cardTex.height };
            Rectangle dest = { card.position.x, card.position.y, src.width * card.currentScale, src.height * card.currentScale };
            Vector2 origin = { dest.width / 2.0f, dest.height / 2.0f };
            DrawTexturePro(card.cardTex, src, dest, origin, 0.0f, WHITE);
            
            // Highlight if selected
            if (card.isSelected) {
                DrawRectangleLinesEx({dest.x - 4, dest.y - 4, dest.width + 8, dest.height + 8}, 4.0f, YELLOW);
            }

            // Draw character idle on top of card
            if (card.idleAnim) {
                Rectangle animSrc = card.idleAnim->getCurrentFrame();
                float animScale = card.idleAnim->getScale() * card.currentScale * 1.5f;
                Rectangle animDest = { card.position.x, card.position.y, 
                                       animSrc.width * animScale, animSrc.height * animScale };
                Vector2 animOrigin = { animDest.width / 2.0f, animDest.height / 2.0f };
                DrawTexturePro(card.idleAnim->getTexture(), animSrc, animDest, animOrigin, 0.0f, WHITE);
            }

            // Draw character name
            int nameFontSize = 30 * card.currentScale;
            Vector2 nameSize = MeasureTextEx(customFont, card.charName.c_str(), nameFontSize, 1.0f);
            Vector2 namePos = { card.position.x - nameSize.x / 2.0f, card.position.y + dest.height / 2.0f - nameSize.y - 10.0f * card.currentScale };
            DrawTextEx(customFont, card.charName.c_str(), namePos, nameFontSize, 1.0f, BLACK);
        }
    }

    // Draw back button
    if (backBtnNormal.id != 0) {
        Texture2D tex = isBackPressed ? backBtnPress : backBtnNormal;
        if (tex.id == 0) tex = backBtnNormal;
        Rectangle src = { 0, 0, (float)tex.width, (float)tex.height };
        DrawTexturePro(tex, src, backBtnRect, {0,0}, 0.0f, WHITE);
    }

    // Draw confirm button
    bool hasSelection = false;
    for (const auto& c : cards) {
        if (c.isSelected) hasSelection = true;
    }
    
    if (confirmBtnNormal.id != 0) {
        Texture2D tex = isConfirmPressed ? confirmBtnPress : confirmBtnNormal;
        Color tint = hasSelection ? WHITE : GRAY;
        Rectangle src = { 0, 0, (float)tex.width, (float)tex.height };
        DrawTexturePro(tex, src, confirmBtnRect, {0,0}, 0.0f, tint);
        
        const char* confirmText = "CONFIRM";
        int fontSize = 30;
        Vector2 textSz = MeasureTextEx(customFont, confirmText, fontSize, 1.0f);
        Vector2 textPos = { confirmBtnRect.x + (confirmBtnRect.width - textSz.x)/2, confirmBtnRect.y + (confirmBtnRect.height - textSz.y)/2 };
        DrawTextEx(customFont, confirmText, textPos, fontSize, 1.0f, hasSelection ? BLACK : DARKGRAY);
    }

    if (isTransitioningIn) transitionIn->Render();
    if (isTransitioningOut) transitionOut->Render();
}
