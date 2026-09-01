#include "EndgameState.h"
#include "MainMenuState.h"
#include "StateCommands.h"
#include "AssetManager.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <cmath>

using json = nlohmann::json;

EndgameState::EndgameState(bool isPvPMode, const std::string& winnerName)
    : isPvPMode_(isPvPMode), winnerName_(winnerName),
      bgTex_({0}), customFont_({0}),
      backBtnRect_({0, 0, 0, 0}), backBtnHitBox_({0, 0, 0, 0}),
      isBtnHovered_(false), isBtnPressed_(false), isReturningToMenu_(false),
      isTransitioningIn_(false), isTransitioningOut_(false), btnAnimTimer_(0.0f),
      screenW_(0), screenH_(0),
      isPlayingSkill_(false), skillTimer_(0.0f)
{
    // Gọi ngay trong constructor: GameState không có Init() trong interface nên
    // StateManager sẽ không bao giờ gọi hộ (đây chính là lỗi cũ khiến màn hình
    // kết thúc trắng trơn và nút quay về không bấm được).
    Init();
}

EndgameState::~EndgameState() {
    Cleanup();
}

void EndgameState::Init() {
    Cleanup();   // cho phép gọi lại nhiều lần mà không rò texture

    screenW_ = (float)GetScreenWidth();
    screenH_ = (float)GetScreenHeight();

    if (isPvPMode_) {
        bgTex_ = LoadTexture("assets/UI_screens/winner_background.png");
    } else {
        bgTex_ = LoadTexture("assets/UI_screens/level_completed_background.png");
    }
    if (bgTex_.id == 0) {
        std::cerr << "[EndgameState] Khong nap duoc anh nen ket thuc man.\n";
    }

    if (isPvPMode_ && !winnerName_.empty()) {
        LoadWinnerAnimations();
    }

    transitionIn_ = std::make_unique<IrisTransition>();
    transitionOut_ = std::make_unique<IrisTransition>();
    isTransitioningIn_ = true;
    isTransitioningOut_ = false;
    transitionIn_->Start(false); // Mở vòng tròn từ tối ra sáng

    btnAnimTimer_ = 0.0f; // Reset đếm giờ cho hiệu ứng nảy nút
    customFont_ = LoadFont("assets/config/kenney-pixel-hu.otf"); // Dùng font giống LoadingState

    RecalculateLayout();
}

// -----------------------------------------------------------------------------
// Nạp hoạt ảnh idle + kỹ năng của nhân vật thắng.
//
// Dùng lại đúng khuôn mẫu của CharacterSelectionState::InitCards(): đọc
// assets/config/characters.json, lấy assetFolder + thông số animation, rồi nhờ
// AssetManager nạp texture (Flyweight — texture đã nạp ở màn chơi sẽ được tái
// dùng, không tốn thêm VRAM).
// -----------------------------------------------------------------------------
void EndgameState::LoadWinnerAnimations() {
    std::ifstream file("assets/config/characters.json");
    if (!file.is_open()) {
        std::cerr << "[EndgameState] Khong mo duoc characters.json\n";
        return;
    }

    json jsonData;
    try {
        file >> jsonData;
    } catch (const std::exception& e) {
        std::cerr << "[EndgameState] characters.json hong: " << e.what() << "\n";
        return;
    }

    if (!jsonData.contains(winnerName_)) {
        std::cerr << "[EndgameState] Khong tim thay nhan vat '" << winnerName_
                  << "' trong characters.json\n";
        return;
    }

    auto& charData = jsonData[winnerName_];
    if (!charData.contains("assetFolder") || !charData.contains("animations")) return;
    std::string assetFolder = charData["assetFolder"].get<std::string>();

    // Lambda dùng chung cho cả idle lẫn kỹ năng — hai khối chỉ khác tên anim.
    auto buildAnim = [&](const std::string& animKey,
                         const std::string& cacheSuffix) -> std::unique_ptr<Animation> {
        if (!charData["animations"].contains(animKey)) return nullptr;
        auto& animData = charData["animations"][animKey];

        std::string texBase = animData["texture"].get<std::string>();
        std::string texKey  = winnerName_ + cacheSuffix + texBase;
        std::string texPath = "assets/" + assetFolder + "/" + texBase + ".png";

        AssetManager::getInstance().loadTexture(texKey, texPath);
        int   frames = animData["frameNum"].get<int>();
        float time   = animData["frameTime"].get<float>();
        float scale  = animData.value("scale", 1.0f);
        return std::make_unique<Animation>(
            AssetManager::getInstance().getTexture(texKey), frames, time, scale);
    };

    winnerIdleAnim_  = buildAnim("idle", "_");
    winnerSkillAnim_ = buildAnim("attack_1", "_skill_");
    if (winnerSkillAnim_) winnerSkillAnim_->setLoop(false);
}

// -----------------------------------------------------------------------------
// Nút quay về Menu ở góc dưới phải, mọi kích thước tính theo chiều cao khung hình.
//
// backBtnHitBox_ luôn giữ kích thước lúc hover, còn backBtnRect_ (vùng vẽ) mới
// co lại khi rời chuột. Nếu để hitbox co theo thì nút sẽ rung liên tục ở mép:
// chuột vào -> nút to ra -> chuột vẫn trong -> ... Đây là mẹo MapSelectionState
// đang dùng cho nút back của nó.
// -----------------------------------------------------------------------------
void EndgameState::RecalculateLayout() {
    float baseWidth  = screenW_ * 0.15f;
    float baseHeight = screenH_ * 0.08f;
    float hoverWidth = baseWidth * 1.05f;
    float hoverHeight = baseHeight * 1.05f;

    float marginX = screenW_ * 0.03f;
    float marginY = screenH_ * 0.10f;

    float targetYHitbox = screenH_ - marginY - hoverHeight;
    float startYHitbox = screenH_ + hoverHeight; // Bắt đầu ở bên dưới mép màn hình
    
    // Phương trình lò xo tắt dần (damped sine wave) cho Y offset
    float scale = 1.0f - expf(-6.0f * btnAnimTimer_) * cosf(15.0f * btnAnimTimer_);
    if (btnAnimTimer_ > 1.0f) scale = 1.0f; // Chốt cứng sau 1 giây để tránh lỗi tính toán
    
    float currentYHitbox = startYHitbox + (targetYHitbox - startYHitbox) * scale;

    backBtnHitBox_ = { screenW_ - marginX - hoverWidth,
                       currentYHitbox,
                       hoverWidth, hoverHeight };

    float btnW = isBtnHovered_ ? hoverWidth : baseWidth;
    float btnH = isBtnHovered_ ? hoverHeight : baseHeight;
    backBtnRect_ = { backBtnHitBox_.x + (hoverWidth - btnW) / 2.0f,
                     backBtnHitBox_.y + (hoverHeight - btnH) / 2.0f,
                     btnW, btnH };
}

void EndgameState::HandleInput() {
    Vector2 mousePos = GetMousePosition();
    bool mousePressed  = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    bool mouseReleased = IsMouseButtonReleased(MOUSE_LEFT_BUTTON);

    isBtnHovered_ = CheckCollisionPointRec(mousePos, backBtnHitBox_);

    if (isBtnHovered_ && mousePressed) {
        isBtnPressed_ = true;
    }

    if (mouseReleased) {
        if (isBtnHovered_ && isBtnPressed_ && !isTransitioningIn_ && !isTransitioningOut_) {
            isTransitioningOut_ = true;
            transitionOut_->Start(true);
        }
        isBtnPressed_ = false;
    }
}

void EndgameState::Process() {
    if (isReturningToMenu_) {
        isReturningToMenu_ = false;
        this->PushStateCommand(
            std::make_unique<ChangeStateCommand>(std::make_unique<MainMenuState>()));
    }
}

void EndgameState::Update(float dt) {
    if (isTransitioningIn_) {
        transitionIn_->Update(dt);
        if (transitionIn_->IsFinished()) isTransitioningIn_ = false;
    } else {
        // Nút bấm chỉ bắt đầu xuất hiện và nảy sau khi mở màn hình xong
        btnAnimTimer_ += dt; 
    }

    if (isTransitioningOut_) {
        transitionOut_->Update(dt);
        if (transitionOut_->IsFinished()) {
            isReturningToMenu_ = true; // Kích hoạt đổi state ở vòng Process
        }
    }

    // Màn hình có thể bị đổi kích thước -> tính lại bố cục nút.
    screenW_ = (float)GetScreenWidth();
    screenH_ = (float)GetScreenHeight();
    RecalculateLayout();

    if (!isPvPMode_) return;

    // Vòng đời hoạt ảnh người thắng, giống hệt island ở màn chọn nhân vật:
    // idle lặp mãi, cứ 3 giây lại có 50% cơ hội chen một lượt kỹ năng.
    if (isPlayingSkill_) {
        if (winnerSkillAnim_) {
            winnerSkillAnim_->update(dt);
            if (winnerSkillAnim_->isFinished()) isPlayingSkill_ = false;
        } else {
            isPlayingSkill_ = false;
        }
    } else {
        if (winnerIdleAnim_) winnerIdleAnim_->update(dt);

        skillTimer_ += dt;
        if (skillTimer_ > 3.0f) {
            if (winnerSkillAnim_ && GetRandomValue(0, 100) < 50) {
                isPlayingSkill_ = true;
                winnerSkillAnim_->resetAnimation();
            }
            skillTimer_ = 0.0f;
        }
    }
}

void EndgameState::Render(float alpha) const {
    // --- Ảnh nền ---
    if (bgTex_.id != 0) {
        Rectangle source = {0.0f, 0.0f, (float)bgTex_.width, (float)bgTex_.height};
        Rectangle dest   = {0.0f, 0.0f, screenW_, screenH_};
        DrawTexturePro(bgTex_, source, dest, {0, 0}, 0.0f, WHITE);
    } else {
        ClearBackground(RAYWHITE);
    }

    // --- Nhân vật thắng đứng trên bục podium (chỉ chế độ 2-Player) ---
    if (isPvPMode_) {
        const Animation* current = (isPlayingSkill_ && winnerSkillAnim_)
                                       ? winnerSkillAnim_.get()
                                       : winnerIdleAnim_.get();
        if (current) {
            Rectangle src = current->getCurrentFrame();
            if (src.height > 0.0f) {
                // Cao bằng kWinnerHeightRatio màn hình, rộng suy ra theo đúng
                // tỉ lệ khung ảnh để nhân vật không bị bóp méo.
                float destH = screenH_ * kWinnerHeightRatio;
                float destW = destH * (src.width / src.height);

                Rectangle dest = { screenW_ * kPodiumCenterX,
                                   screenH_ * kPodiumTopY,
                                   destW, destH };
                Vector2 origin = { destW / 2.0f, destH };  // neo GIỮA-ĐÁY: chân chạm mặt bục
                DrawTexturePro(current->getTexture(), src, dest, origin, 0.0f, WHITE);
            }
        }
    } else {
        // --- Hiệu ứng chữ LEVEL COMPLETED cho chế độ 1-Player ---
        float scale = 1.0f - expf(-6.0f * btnAnimTimer_) * cosf(15.0f * btnAnimTimer_);
        if (btnAnimTimer_ > 1.0f) scale = 1.0f;
        
        float fontSize = screenH_ * 0.15f; // Chữ rất lớn
        const char* msg = "LEVEL COMPLETED!";
        
        Vector2 textSize = {0,0};
        if (customFont_.texture.id != 0) {
            textSize = MeasureTextEx(customFont_, msg, fontSize, 1.0f);
        } else {
            textSize.x = (float)MeasureText(msg, (int)fontSize);
            textSize.y = fontSize;
        }

        // Vị trí đích ở chính giữa màn hình
        float targetY = screenH_ / 2.0f - textSize.y / 2.0f;
        float startY = -textSize.y; // Rơi từ trên cùng màn hình xuống
        float currentY = startY + (targetY - startY) * scale;
        float textX = screenW_ / 2.0f - textSize.x / 2.0f;

        // Vẽ bóng đen
        if (customFont_.texture.id != 0) {
            DrawTextEx(customFont_, msg, {textX + 6.0f, currentY + 6.0f}, fontSize, 1.0f, Color{0, 0, 0, 150});
            DrawTextEx(customFont_, msg, {textX, currentY}, fontSize, 1.0f, GREEN); // Xanh lá
        } else {
            DrawText(msg, (int)textX + 6, (int)currentY + 6, (int)fontSize, Color{0, 0, 0, 150});
            DrawText(msg, (int)textX, (int)currentY, (int)fontSize, GREEN);
        }
    }

    // --- Nút quay về Menu ---
    Color btnColor = isBtnHovered_ ? (isBtnPressed_ ? Color{46, 125, 50, 255} : Color{76, 175, 80, 255}) : Color{56, 142, 60, 255};
    Color borderColor = isBtnHovered_ ? (isBtnPressed_ ? Color{27, 94, 32, 255} : Color{56, 142, 60, 255}) : Color{38, 110, 42, 255};
    
    // Đổ bóng (Shadow) dày và đậm hơn một chút
    Rectangle shadowRect = backBtnRect_;
    shadowRect.x += 6.0f;
    shadowRect.y += 6.0f;
    DrawRectangleRounded(shadowRect, 0.4f, 16, Color{0, 0, 0, 120});

    // Nút chính
    DrawRectangleRounded(backBtnRect_, 0.4f, 16, btnColor);

    // Viền nút
    DrawRectangleRoundedLines(backBtnRect_, 0.4f, 16, 4.0f, borderColor);

    // Chữ "Return"
    const char* btnText = "Return";
    int fontSize = (int)(backBtnRect_.height * 0.5f);
    int textW = MeasureText(btnText, fontSize);
    DrawText(btnText, 
             (int)(backBtnRect_.x + backBtnRect_.width / 2.0f - textW / 2.0f), 
             (int)(backBtnRect_.y + backBtnRect_.height / 2.0f - fontSize / 2.0f), 
             fontSize, WHITE);

    // --- Vẽ hiệu ứng Transition nằm đè lên trên cùng ---
    if (isTransitioningIn_) transitionIn_->Render();
    if (isTransitioningOut_) transitionOut_->Render();
}

void EndgameState::Cleanup() {
    if (bgTex_.id != 0) {
        UnloadTexture(bgTex_);
        bgTex_ = {0};
    }
    if (customFont_.texture.id != 0) {
        UnloadFont(customFont_);
        customFont_.texture.id = 0;
    }
    // winnerIdleAnim_/winnerSkillAnim_ chỉ TRỎ tới texture do AssetManager sở
    // hữu, nên chỉ cần thả con trỏ, tuyệt đối không UnloadTexture ở đây.
    winnerIdleAnim_.reset();
    winnerSkillAnim_.reset();
}
