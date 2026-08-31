#include "MapSelectionState.h"
#include "StateCommands.h"
#include "MainMenuState.h"
#include "LoadingState.h"
#include "CharacterSelectionState.h"
#include "WorldCatalog.h"
#include "SaveManager.h"
#include "BaseLevelState.h"
#include <iostream>
#include <cmath>

MapSelectionState::MapSelectionState(MapSelectionMode mode) : currentMode(mode) {
    backgroundTex = LoadTexture("assets/UI_screens/map_selection.png");
    customFont = LoadFont("assets/config/kenney-pixel-hu.otf");
    
    if (backgroundTex.id == 0) {
        std::cerr << "WARNING: Could not load assets/UI_screens/map_selection.png" << std::endl;
    }

    // Back Button
    backBtnNormal = LoadTexture("assets/UI_screens/menu_btn_back.png");
    backBtnPress = LoadTexture("assets/UI_screens/menu_btn_back_press.png");
    isBackHovered = false;
    isBackPressed = false;
    isBackClicked = false;
    isReturningToMenu = false;
    
    // Position back button at top-left will be calculated in HandleInput

    transitionIn = std::make_unique<IrisTransition>();
    transitionIn->Start(false); // expanding
    isTransitioningIn = true;
    
    transitionOut = std::make_unique<IrisTransition>();
    isTransitioningOut = false;
    
    targetWorldIndex = -1;

    InitNodes();

    // Panel NEW GAME/LOAD GAME chỉ có nghĩa ở chế độ một người chơi — chế độ
    // PvP không có hệ thống lưu, bấm world là vào thẳng như cũ.
    if (currentMode == MapSelectionMode::SinglePlayer) {
        InitSavePanels();
    }
}

// -----------------------------------------------------------------------------
// Dựng hai panel overlay và nối dây các callback giữa chúng.
//
// Chuỗi sự kiện được thiết kế theo hướng "panel không biết gì về GameState":
// panel chỉ báo "người dùng vừa bấm X", còn quyết định chuyển cảnh là của
// MapSelectionState. Nhờ vậy hai panel này tái dùng được ở màn hình khác.
// -----------------------------------------------------------------------------
void MapSelectionState::InitSavePanels() {
    float sw = (float)GetScreenWidth();
    float sh = (float)GetScreenHeight();

    worldActionPanel_ = std::make_unique<WorldActionPanel>();
    worldActionPanel_->Init(sw, sh, customFont);   // customFont là tài nguyên MƯỢN

    saveVersionPanel_ = std::make_unique<SaveVersionPanel>();
    saveVersionPanel_->Init(sw, sh, customFont);

    // --- NEW GAME: đóng panel, bắt đầu hiệu ứng chuyển cảnh ---
    worldActionPanel_->SetOnNewGame([this]() {
        this->worldActionPanel_->Close();
        this->pendingAction_ = MapSelectionAction::NewGame;
        this->isReturningToMenu = false;
        this->isTransitioningOut = true;
        this->transitionOut->Start(true);
    });

    // --- LOAD GAME: đổi sang panel danh sách bản lưu (không chuyển cảnh) ---
    worldActionPanel_->SetOnLoadGame([this]() {
        int idx = this->targetWorldIndex;
        this->worldActionPanel_->Close();
        this->saveVersionPanel_->Open(idx,
                                      WorldCatalog::getInstance().displayName(idx),
                                      SaveManager::getInstance().listVersions(idx));
    });

    // --- Đóng panel action: quay lại màn chọn map bình thường ---
    worldActionPanel_->SetOnCloseCallback([this]() {
        this->targetWorldIndex = -1;
    });

    // --- Chọn một bản lưu rồi bấm LOAD ---
    saveVersionPanel_->SetOnLoad([this](const SaveSlotInfo& slot) {
        GameSaveData data;
        if (!SaveManager::getInstance().loadVersion(slot, data)) {
            std::cerr << "[MapSelectionState] Khong nap duoc ban luu: " << slot.filePath << "\n";
            return;   // giữ panel mở để người chơi chọn bản khác
        }
        this->pendingSave_ = data;
        this->saveVersionPanel_->Close();
        this->pendingAction_ = MapSelectionAction::LoadGame;
        this->isReturningToMenu = false;
        this->isTransitioningOut = true;
        this->transitionOut->Start(true);
    });

    // --- Chọn một bản lưu rồi bấm DELETE ---
    saveVersionPanel_->SetOnDelete([this](const SaveSlotInfo& slot) {
        SaveManager::getInstance().deleteVersion(slot);
        // Nạp lại danh sách từ đĩa thay vì tự xoá phần tử trong vector: nếu
        // việc xoá file thất bại (file đang bị khoá) thì dòng đó phải còn lại,
        // đĩa mới là nguồn chân lý.
        int idx = this->saveVersionPanel_->GetWorldIndex();
        this->saveVersionPanel_->Refresh(SaveManager::getInstance().listVersions(idx));
    });

    saveVersionPanel_->SetOnCloseCallback([this]() {
        this->targetWorldIndex = -1;
    });
}

bool MapSelectionState::IsAnyPanelOpen() const {
    return (worldActionPanel_ && worldActionPanel_->IsOpen()) ||
           (saveVersionPanel_ && saveVersionPanel_->IsOpen());
}

MapSelectionState::~MapSelectionState() {
    UnloadFont(customFont);
    UnloadTexture(backgroundTex);
    UnloadTexture(backBtnNormal);
    UnloadTexture(backBtnPress);
    for (auto& node : mapNodes) {
        UnloadTexture(node.tex);
    }
}

void MapSelectionState::InitNodes() {
    const char* imagePaths[6] = {
        "assets/UI_screens/building_1.png",
        "assets/UI_screens/building_2.png",
        "assets/UI_screens/building_3.png",
        "assets/UI_screens/building_4.png",
        "assets/UI_screens/building_05.png",
        "assets/UI_screens/building_6.png"
    };

    // Spread them out for a 1280x720 screen
    float sw = (float)GetScreenWidth();
    float sh = (float)GetScreenHeight();

    Vector2 positions[6] = {
        {sw / 2.0f * 1.04f, sh / 2.0f * 0.90f},
        {sw / 4.0f * 1.50f, sh / 4.0f * 1.10f},
        {sw / 4.0f * 2.75f, sh / 4.0f * 1.32f},
        {sw / 4.0f * 3.05f, sh / 4.0f * 2.45f},
        {sw / 4.0f * 1.10f, sh / 4.0f * 2.20f},
        {sw / 4.0f * 2.25f, sh / 4.0f * 2.80f}
    };

    for (int i = 0; i < 6; ++i) {
        MapNode node;
        node.tex = LoadTexture(imagePaths[i]);
        if (node.tex.id == 0) {
            std::cerr << "WARNING: Could not load " << imagePaths[i] << std::endl;
        } else {
            std::cout << "Successfully loaded " << imagePaths[i] << " (ID: " << node.tex.id << ")" << std::endl;
        }
        node.position = positions[i];
        
        node.baseScale = 0.45f; // Increased scale just in case it was too small
        node.currentScale = node.baseScale;
        node.targetScale = node.baseScale;
        node.worldIndex = i + 1; // World 1 to 6
        node.isHovered = false;
        node.isClicked = false;
        
        float w = node.tex.width * node.currentScale;
        float h = node.tex.height * node.currentScale;
        node.hitBox = {node.position.x - w/2.0f, node.position.y - h/2.0f, w, h};
        
        mapNodes.push_back(node);
    }
}

void MapSelectionState::HandleInput() {
    if (isTransitioningIn || isTransitioningOut) return;

    Vector2 mousePos = GetMousePosition();
    bool mousePressed = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    bool mouseReleased = IsMouseButtonReleased(MOUSE_LEFT_BUTTON);

    // Panel overlay NUỐT TOÀN BỘ input: nếu không chặn ở đây, cú click vào nút
    // NEW GAME sẽ đồng thời rơi trúng building nằm ngay dưới panel và mở lại
    // panel cho world khác.
    if (IsAnyPanelOpen()) {
        if (worldActionPanel_ && worldActionPanel_->IsOpen())
            worldActionPanel_->HandleInput(mousePos, mousePressed, mouseReleased);
        if (saveVersionPanel_ && saveVersionPanel_->IsOpen())
            saveVersionPanel_->HandleInput(mousePos, mousePressed, mouseReleased);

        // Xoá trạng thái hover của các node để chúng không "sáng" xuyên qua panel
        isBackHovered = false;
        for (auto& node : mapNodes) node.isHovered = false;
        return;
    }

    // Back button logic
    float baseSize = 48.0f;
    float hoverSize = 56.0f;
    
    // Fixed hitbox based on hover size to prevent jitter
    Rectangle hitBox = { 10.0f, 10.0f, hoverSize, hoverSize };
    isBackHovered = CheckCollisionPointRec(mousePos, hitBox);
    
    if (backBtnNormal.id != 0) {
        float btnSize = isBackHovered ? hoverSize : baseSize;
        float offset = (hoverSize - btnSize) / 2.0f;
        backBtnRect = { 10.0f + offset, 10.0f + offset, btnSize, btnSize };
        
        if (isBackHovered && mousePressed) {
            isBackPressed = true;
        }
    }
    
    if (mouseReleased) {
        if (isBackHovered && isBackPressed) {
            isBackClicked = true;
        }
        isBackPressed = false;
    }

    // Node logic
    for (auto& node : mapNodes) {
        node.isHovered = CheckCollisionPointRec(mousePos, node.hitBox);
        if (node.isHovered && mouseReleased) {
            node.isClicked = true;
        }
    }
}

void MapSelectionState::Process() {
    if (isTransitioningIn || isTransitioningOut) return;

    // Panel đang mở -> mọi click đã được nó tiêu thụ, không xử lý node.
    if (IsAnyPanelOpen()) return;

    if (isBackClicked) {
        isBackClicked = false;
        isReturningToMenu = true;
        pendingAction_ = MapSelectionAction::BackToMenu;
        isTransitioningOut = true;
        transitionOut->Start(true); // shrinking
        return;
    }

    for (auto& node : mapNodes) {
        if (node.isClicked) {
            node.isClicked = false;
            targetWorldIndex = node.worldIndex;
            isReturningToMenu = false;

            if (currentMode == MapSelectionMode::SinglePlayer && worldActionPanel_) {
                // 1-Player: hỏi NEW GAME hay LOAD GAME trước, CHƯA chuyển cảnh.
                worldActionPanel_->Open(node.worldIndex,
                                        WorldCatalog::getInstance().displayName(node.worldIndex));
            } else {
                // PvP: giữ nguyên hành vi cũ, vào thẳng màn chọn nhân vật.
                pendingAction_ = MapSelectionAction::NewGame;
                isTransitioningOut = true;
                transitionOut->Start(true); // shrinking
            }
            break; // only process one click
        }
    }
}

void MapSelectionState::Update(float dt) {
    if (isTransitioningIn) {
        transitionIn->Update(dt);
        if (transitionIn->IsFinished()) {
            isTransitioningIn = false;
        }
    }

    // Panel tự cập nhật animation và phát callback ở đây (giai đoạn Update).
    if (worldActionPanel_ && worldActionPanel_->IsOpen()) worldActionPanel_->Update(dt);
    if (saveVersionPanel_ && saveVersionPanel_->IsOpen()) saveVersionPanel_->Update(dt);

    if (isTransitioningOut) {
        transitionOut->Update(dt);
        if (transitionOut->IsFinished()) {
            std::function<std::unique_ptr<GameState>()> factory;
            int idx = targetWorldIndex;
            MapSelectionMode curMode = currentMode;

            switch (pendingAction_) {
            case MapSelectionAction::BackToMenu:
                factory = []() { return std::make_unique<MainMenuState>(); };
                break;

            case MapSelectionAction::LoadGame: {
                // Vào THẲNG màn chơi, bỏ qua Character Selection: nhân vật đã
                // nằm sẵn trong bản lưu. WorldCatalog dựng đúng World0XState.
                GameSaveData save = pendingSave_;
                factory = [idx, save]() -> std::unique_ptr<GameState> {
                    auto state = WorldCatalog::getInstance().createLoaded(idx, save);
                    // Bản lưu hỏng/world lạ -> quay về menu thay vì trả nullptr
                    // cho StateManager (sẽ làm rỗng stack và thoát game).
                    if (!state) return std::make_unique<MainMenuState>();
                    return state;
                };
                break;
            }

            case MapSelectionAction::NewGame:
            default:
                if (curMode == MapSelectionMode::SinglePlayer) {
                    // WorldCatalog thay cho khối switch(idx) 6 nhánh trước đây.
                    LevelFactory levelFact = [idx](std::string p1, std::string /*p2*/) -> std::unique_ptr<GameState> {
                        auto state = WorldCatalog::getInstance().createNew(idx, p1);
                        if (!state) return WorldCatalog::getInstance().createNew(1, p1);
                        return state;
                    };
                    factory = [levelFact, curMode]() {
                        return std::make_unique<CharacterSelectionState>(
                            1, levelFact,
                            [curMode]() { return std::make_unique<MapSelectionState>(curMode); });
                    };
                } else {
                    LevelFactory levelFact = [idx](std::string p1, std::string p2) -> std::unique_ptr<GameState> {
                        std::string mapPath = "assets/maps/pvp_map0" + std::to_string(idx) + "/world0" + std::to_string(idx) + ".ldtk";
                        return std::make_unique<BaseLevelState>(mapPath, "", p1, p2, true);
                    };
                    factory = [levelFact, curMode]() {
                        return std::make_unique<CharacterSelectionState>(
                            2, levelFact,
                            [curMode]() { return std::make_unique<MapSelectionState>(curMode); });
                    };
                }
                break;
            }

            this->PushStateCommand(std::make_unique<::ChangeStateCommand>(std::make_unique<LoadingState>(factory, 1.0f)));
            return;
        }
    }

    for (auto& node : mapNodes) {
        if (node.isHovered) {
            node.targetScale = node.baseScale * 1.15f; 
        } else {
            node.targetScale = node.baseScale;
        }

        // Smooth interpolation for the scale
        float lerpFactor = 12.0f * dt;
        if (lerpFactor > 1.0f) lerpFactor = 1.0f; // Prevent overshoot on lag spikes
        node.currentScale += (node.targetScale - node.currentScale) * lerpFactor;

        // Update hitBox centered on the position
        float w = node.tex.width * node.currentScale;
        float h = node.tex.height * node.currentScale;
        node.hitBox = {node.position.x - w/2.0f, node.position.y - h/2.0f, w, h};
    }
}

void MapSelectionState::Render(float alpha) const {
    ClearBackground(BLACK);

    if (backgroundTex.id != 0) {
        Rectangle src = { 0, 0, (float)backgroundTex.width, (float)backgroundTex.height };
        Rectangle dest = { 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() };
        DrawTexturePro(backgroundTex, src, dest, {0, 0}, 0.0f, WHITE);
    } else {
        DrawText("MAP SELECTION (Missing Image)", GetScreenWidth() / 2 - 200, GetScreenHeight() / 2, 30, WHITE);
    }

    // Draw Title
    std::string titleStr = (currentMode == MapSelectionMode::SinglePlayer) ? "1-PLAYER MODE" : "2-PLAYER MODE";
    float fontSize = 80.0f;
    Vector2 textSize = MeasureTextEx(customFont, titleStr.c_str(), fontSize, 2.0f);
    Vector2 titlePos = { (GetScreenWidth() - textSize.x) / 2.0f, 40.0f };
    
    // Draw text with shadow
    DrawTextEx(customFont, titleStr.c_str(), {titlePos.x + 4.0f, titlePos.y + 4.0f}, fontSize, 2.0f, {0, 0, 0, 200});
    DrawTextEx(customFont, titleStr.c_str(), titlePos, fontSize, 2.0f, {248, 34, 0, 255});

    for (const auto& node : mapNodes) {
        if (node.tex.id != 0) {
            Rectangle src = { 0, 0, (float)node.tex.width, (float)node.tex.height };
            Rectangle dest = { node.position.x, node.position.y, src.width * node.currentScale, src.height * node.currentScale };
            Vector2 origin = { dest.width / 2.0f, dest.height / 2.0f }; 
            DrawTexturePro(node.tex, src, dest, origin, 0.0f, WHITE);
        }
    }
    
    // Draw back button
    if (backBtnNormal.id != 0) {
        Texture2D tex = isBackPressed ? backBtnPress : backBtnNormal;
        if (tex.id == 0) tex = backBtnNormal;
        Rectangle src = { 0, 0, (float)tex.width, (float)tex.height };
        DrawTexturePro(tex, src, backBtnRect, {0,0}, 0.0f, WHITE);
    }

    // Panel overlay vẽ SAU các node/nút back (để đè lên) nhưng TRƯỚC hiệu ứng
    // chuyển cảnh (để iris vẫn phủ được lên tất cả).
    if (worldActionPanel_ && worldActionPanel_->IsOpen()) worldActionPanel_->Render();
    if (saveVersionPanel_ && saveVersionPanel_->IsOpen()) saveVersionPanel_->Render();

    if (isTransitioningIn) {
        transitionIn->Render();
    }

    if (isTransitioningOut) {
        transitionOut->Render();
    }
}
