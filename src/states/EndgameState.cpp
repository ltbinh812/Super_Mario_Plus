#include "EndgameState.h"
#include "MainMenuState.h"
#include "StateCommands.h"
#include "AssetManager.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

using json = nlohmann::json;

EndgameState::EndgameState(bool isPvPMode, const std::string& winnerName)
    : isPvPMode_(isPvPMode), winnerName_(winnerName),
      bgTex_({0}), btnNormalTex_({0}), btnPressTex_({0}),
      backBtnRect_({0, 0, 0, 0}), backBtnHitBox_({0, 0, 0, 0}),
      isBtnHovered_(false), isBtnPressed_(false), isReturningToMenu_(false),
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

    btnNormalTex_ = LoadTexture("assets/UI_screens/undo_button_2.png");
    btnPressTex_  = LoadTexture("assets/UI_screens/undo_button_2.png"); // chưa có ảnh press riêng

    if (isPvPMode_ && !winnerName_.empty()) {
        LoadWinnerAnimations();
    }

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
    float baseSize  = screenH_ * 0.10f;
    float hoverSize = baseSize * 1.1f;

    float marginX = screenW_ * 0.05f;
    float marginY = screenH_ * 0.05f;

    backBtnHitBox_ = { screenW_ - marginX - hoverSize,
                       screenH_ - marginY - hoverSize,
                       hoverSize, hoverSize };

    float btnSize = isBtnHovered_ ? hoverSize : baseSize;
    backBtnRect_ = { backBtnHitBox_.x + (hoverSize - btnSize) / 2.0f,
                     backBtnHitBox_.y + (hoverSize - btnSize) / 2.0f,
                     btnSize, btnSize };
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
        // Chỉ GHI NHẬN ý định; việc chuyển state để Process() làm, đúng phân
        // tách 4 giai đoạn của dự án.
        if (isBtnHovered_ && isBtnPressed_) {
            isReturningToMenu_ = true;
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
        const char* msg = isPvPMode_ ? "WINNER!" : "LEVEL COMPLETED!";
        int fs = 40;
        int tw = MeasureText(msg, fs);
        DrawText(msg, (int)(screenW_ / 2.0f - tw / 2.0f), (int)(screenH_ / 2.0f), fs, BLACK);
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
    }

    // --- Nút quay về Menu ---
    Texture2D texToDraw = (isBtnPressed_ && isBtnHovered_) ? btnPressTex_ : btnNormalTex_;
    if (texToDraw.id != 0) {
        Rectangle src = {0.0f, 0.0f, (float)texToDraw.width, (float)texToDraw.height};
        DrawTexturePro(texToDraw, src, backBtnRect_, {0, 0}, 0.0f, WHITE);
    }
}

void EndgameState::Cleanup() {
    if (bgTex_.id != 0) {
        UnloadTexture(bgTex_);
        bgTex_ = {0};
    }
    if (btnNormalTex_.id != 0) {
        UnloadTexture(btnNormalTex_);
        btnNormalTex_ = {0};
    }
    if (btnPressTex_.id != 0) {
        UnloadTexture(btnPressTex_);
        btnPressTex_ = {0};
    }
    // winnerIdleAnim_/winnerSkillAnim_ chỉ TRỎ tới texture do AssetManager sở
    // hữu, nên chỉ cần thả con trỏ, tuyệt đối không UnloadTexture ở đây.
    winnerIdleAnim_.reset();
    winnerSkillAnim_.reset();
}
