#include "CharacterSelectionState.h"
#include "StateCommands.h"
#include "LoadingState.h"
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include "AssetManager.h"
#include "raylib.h"

using json = nlohmann::json;

CharacterSelectionState::CharacterSelectionState(int numPlayers, LevelFactory factory, std::function<std::unique_ptr<GameState>()> backStateFactory)
    : numPlayersRequired(numPlayers), currentPlayerSelecting(1), nextStateFactory(std::move(factory)), backStateFactory(std::move(backStateFactory))
{
    isTransitioningIn = true;
    isTransitioningOut = false;
    isTransitioningToPlayer2 = false;
    transitionIn = std::make_unique<IrisTransition>();
    transitionOut = std::make_unique<IrisTransition>();

    backgroundTex = LoadTexture("assets/UI_screens/map_selection.png"); // ảnh nền tĩnh dự phòng
    // Ảnh nền động: 14 khung, mỗi khung 150ms trong file gốc -> ~6.7 fps.
    backgroundGif.Load("assets/UI_screens/character_selection.gif", 6.7f);
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
    islandAnimOffsetY = (float)GetScreenHeight(); // Start below screen
    
    introTimer = 0.0f;

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
        {screenW * 0.52f, screenH * 0.28f},
        {screenW * 0.70f, screenH * 0.28f},
        {screenW * 0.88f, screenH * 0.28f},
        {screenW * 0.52f, screenH * 0.70f},
        {screenW * 0.70f, screenH * 0.70f},
        {screenW * 0.88f, screenH * 0.70f}
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
        card.baseScale = (float)GetScreenHeight() / (float)cardBgTex.height * 0.36f;
        card.currentScale = card.baseScale;
        card.targetScale = card.baseScale;
        card.isHovered = false;
        card.isSelected = false;
        card.animOffsetY = (float)GetScreenHeight(); // Start below screen
        
        float w = card.cardTex.width * card.baseScale;
        float h = card.cardTex.height * card.baseScale;
        // std::cout << "checker: " << card.baseScale << " " << GetScreenHeight() << " " << 
        //     cardBgTex.height << " " << h << std::endl;
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
                        isTransitioningToPlayer2 = true;
                        isTransitioningOut = true;
                        transitionOut->Start(true);
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
    // Nền động chạy liên tục, kể cả trong lúc chuyển cảnh, để không bị khựng.
    backgroundGif.Update(dt);

    if (isTransitioningIn) {
        transitionIn->Update(dt);
        if (transitionIn->IsFinished()) isTransitioningIn = false;
    }

    if (isTransitioningOut) {
        transitionOut->Update(dt);
        if (transitionOut->IsFinished()) {
            if (isReturningToMenu) {
                if (backStateFactory) {
                    this->PushStateCommand(std::make_unique<::ChangeStateCommand>(backStateFactory()));
                } else {
                    this->PushStateCommand(std::make_unique<::PopStateCommand>());
                }
                return;
            }
            
            if (isTransitioningToPlayer2) {
                currentPlayerSelecting = 2;
                for (auto& c : cards) c.isSelected = false;
                isTransitioningToPlayer2 = false;
                isTransitioningOut = false;
                isTransitioningIn = true;
                introTimer = 0.0f;
                islandAnimOffsetY = (float)GetScreenHeight();
                for (auto& c : cards) c.animOffsetY = (float)GetScreenHeight();
                transitionIn->Start(false); // Restart transition in
                return;
            }
            
            std::string p1 = player1Choice;
            std::string p2 = player2Choice;
            LevelFactory factory = nextStateFactory;
            this->PushStateCommand(std::make_unique<::ChangeStateCommand>(
                std::make_unique<LoadingState>([factory, p1, p2]() { return factory(p1, p2); }, 1.0f)
            ));
            return;
        }
    }

    introTimer += dt;

    // Animate Island (Spring bounce)
    if (introTimer > 0.0f) {
        islandAnimOffsetY = (float)GetScreenHeight() * exp(-4.0f * introTimer) * cos(12.0f * introTimer);
    }

    // Update cards animations and logic
    for (size_t i = 0; i < cards.size(); i++) {
        auto& card = cards[i];
        
        // Intro animation
        float cardT = introTimer - 0.1f * (i + 1); // Delay each card by 0.1s
        if (cardT > 0.0f) {
            card.animOffsetY = (float)GetScreenHeight() * exp(-4.0f * cardT) * cos(12.0f * cardT);
        }
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
        card.hitBox = {card.position.x - w/2.0f, card.position.y + card.animOffsetY - h/2.0f, w, h};
        
        if (card.idleAnim) {
            card.idleAnim->update(dt);
        }
    }

    UpdateIslandLogic(dt);
}

void CharacterSelectionState::Render(float alpha) const {
    ClearBackground(BLACK);

    // Ảnh nền động; nếu GIF không nạp được thì rơi về ảnh tĩnh cũ.
    if (backgroundGif.IsLoaded()) {
        backgroundGif.DrawFullscreen();
    } else if (backgroundTex.id != 0) {
        Rectangle src = { 0, 0, (float)backgroundTex.width, (float)backgroundTex.height };
        Rectangle dest = { 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() };
        DrawTexturePro(backgroundTex, src, dest, {0, 0}, 0.0f, WHITE);
    }

    // Lớp phủ đen 50% làm dịu ảnh nền, để tiêu đề và các thẻ nhân vật nổi lên.
    // Alpha 128/255 ≈ 50%. Vẽ ngay sau nền và TRƯỚC mọi thứ khác nên chỉ ảnh
    // nền bị tối, còn đảo bay + thẻ nhân vật vẫn giữ nguyên độ tương phản.
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Color{0, 0, 0, 128});

    // Draw Title
    std::string titleStr = (currentPlayerSelecting == 1) ? "PLAYER 1: SELECT CHARACTER" : "PLAYER 2: SELECT CHARACTER";
    float fontSize = 70.0f;
    Vector2 textSize = MeasureTextEx(customFont, titleStr.c_str(), fontSize, 2.0f);
    Vector2 titlePos = { (GetScreenWidth() - textSize.x) / 2.0f, 30.0f };
    DrawTextEx(customFont, titleStr.c_str(), {titlePos.x + 4.0f, titlePos.y + 4.0f}, fontSize, 2.0f, {0, 0, 0, 200});
    DrawTextEx(customFont, titleStr.c_str(), titlePos, fontSize, 2.0f, (currentPlayerSelecting == 1 ? Color({0, 255, 0, 255}) : Color({255, 102, 0, 255})));

    // Draw Island
    if (islandTex.id != 0) {
        Rectangle src = { 0, 0, (float)islandTex.width, (float)islandTex.height };
        float islandW = islandTex.width * islandScale;
        float islandH = islandTex.height * islandScale;
        Vector2 origin = { islandW / 2.0f, islandH / 2.0f };
        DrawTexturePro(islandTex, src, {islandPos.x, islandPos.y + islandAnimOffsetY, islandW, islandH}, origin, 0.0f, WHITE);
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
            float scale = islandTex.width * islandScale / src.width * 0.5f;
            Rectangle dest = { islandPos.x, islandPos.y + islandAnimOffsetY - (45.0f * islandScale), src.width * scale, src.height * scale };
            Vector2 origin = { dest.width / 2.0f, dest.height / 2.0f };
            DrawTexturePro(currentAnim->getTexture(), src, dest, origin, 0.0f, WHITE);
        }
    }

    // Draw Cards
    for (const auto& card : cards) {
        if (card.cardTex.id != 0) {
            Rectangle src = { 0, 0, (float)card.cardTex.width, (float)card.cardTex.height };
            Rectangle dest = { card.position.x, card.position.y + card.animOffsetY, src.width * card.currentScale, src.height * card.currentScale };
            Vector2 origin = { dest.width / 2.0f, dest.height / 2.0f };
            DrawTexturePro(card.cardTex, src, dest, origin, 0.0f, WHITE);
            


            // Draw character idle on top of card
            if (card.idleAnim) {
                Rectangle animSrc = card.idleAnim->getCurrentFrame();
                float animScale = (card.hitBox.width * 0.85f) / (float)animSrc.width;
                Rectangle animDest = { card.position.x, card.position.y + card.animOffsetY, 
                                       animSrc.width * animScale, animSrc.height * animScale };
                Vector2 animOrigin = { animDest.width / 2.0f, animDest.height / 2.0f };
                DrawTexturePro(card.idleAnim->getTexture(), animSrc, animDest, animOrigin, 0.0f, WHITE);
            }

            // Draw character name
            int nameFontSize = 30 * card.currentScale;
            Vector2 nameSize = MeasureTextEx(customFont, card.charName.c_str(), nameFontSize, 1.0f);
            Vector2 namePos = { card.position.x - nameSize.x / 2.0f, card.position.y + card.animOffsetY + dest.height / 2.0f - nameSize.y - 10.0f * card.currentScale };
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
