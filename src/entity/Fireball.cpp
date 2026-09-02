#include "Fireball.h"
#include "AssetManager.h"
#include <iostream>
#include <cmath>
#include <unordered_map>
#include <vector>

// =============================================================================
// Canh khung theo NỘI DUNG ảnh (cfg.alignFramesByContent).
//
// Với mỗi khung của sprite sheet, tìm hộp bao của các điểm ảnh không trong
// suốt, rồi tính tâm của hộp đó theo tỉ lệ khung — CẢ TRỤC X LẪN TRỤC Y.
// Lúc vẽ, dịch ảnh sao cho tâm nội dung của khung nằm đúng vị trí quả đạn.
//
// Nhờ vậy phi tiêu (nằm lệch hẳn sang trái, lưng chừng phía dưới khung) và quả
// cầu nổ (nằm giữa khung, cao hơn) xuất hiện ở CÙNG một chỗ trong thế giới —
// điểm cuối của phi tiêu chính là điểm đầu của quả cầu.
//
// Đọc ngược texture từ GPU là thao tác đắt, nên kết quả được nhớ theo tên
// texture: mỗi sprite sheet chỉ phân tích một lần cho cả phiên chơi.
// =============================================================================
static const std::vector<Vector2>* GetFrameContentCenters(const std::string& texName,
                                                          const Texture2D& tex,
                                                          int frameNum) {
    static std::unordered_map<std::string, std::vector<Vector2>> cache;

    auto it = cache.find(texName);
    if (it != cache.end()) return &it->second;

    std::vector<Vector2> centers(frameNum > 0 ? frameNum : 1, Vector2{0.5f, 0.5f});

    if (tex.id != 0 && frameNum > 0) {
        Image img = LoadImageFromTexture(tex);
        if (img.data != nullptr) {
            ImageFormat(&img, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
            const Color* px = reinterpret_cast<const Color*>(img.data);
            const int fw = img.width / frameNum;

            for (int f = 0; f < frameNum && fw > 0; ++f) {
                int minX = fw, maxX = -1, minY = img.height, maxY = -1;
                for (int y = 0; y < img.height; ++y) {
                    const int rowBase = y * img.width + f * fw;
                    for (int x = 0; x < fw; ++x) {
                        if (px[rowBase + x].a > 8) {
                            if (x < minX) minX = x;
                            if (x > maxX) maxX = x;
                            if (y < minY) minY = y;
                            if (y > maxY) maxY = y;
                        }
                    }
                }
                if (maxX >= minX && maxY >= minY) {
                    centers[f].x = ((minX + maxX) * 0.5f) / static_cast<float>(fw);
                    centers[f].y = ((minY + maxY) * 0.5f) / static_cast<float>(img.height);
                }
            }
            UnloadImage(img);
        }
    }

    auto res = cache.emplace(texName, std::move(centers));
    return &res.first->second;
}

static CharacterBaseStats getFireballBaseStats(const FireballConfig& config) {
    CharacterBaseStats bs;
    bs.name = "Fireball";
    bs.moveVelocity = config.speed;
    bs.gravityScale = config.gravityScale;
    bs.physicsBox = {config.hitboxW, config.hitboxH};
    return bs;
}

static CharacterRuntimeStats getFireballRuntimeStats(bool isFacingRight, const FireballConfig& config) {
    CharacterRuntimeStats rs;
    rs.velocity.x = isFacingRight ? config.speed : -config.speed;
    rs.physicsBox = {config.hitboxW, config.hitboxH};
    return rs;
}

static CharacterWorldStats getFireballWorldStats(Vector2 startPos, bool isFacingRight) {
    CharacterWorldStats ws;
    ws.position = startPos;
    ws.isFacingRight = isFacingRight;
    ws.animation = nullptr;
    return ws;
}

Fireball::Fireball(Vector2 startPos, bool isFacingRight, const FireballConfig& config, Entity* spawner)
    : Entity(getFireballBaseStats(config), getFireballRuntimeStats(isFacingRight, config), getFireballWorldStats(startPos, isFacingRight)),
      lifetime(config.lifetime),
      attackPower(config.damage),
      curveAmplitude(config.curveAmplitude),
      curveFrequency(config.curveFrequency),
      originY(startPos.y),
      beamFromOwner(config.beamFromOwner),
      alignFramesByContent(config.alignFramesByContent),
      textureName(config.textureName),
      soundKey(config.soundKey),
      spawner(spawner),
      hitboxOffsetX(config.hitboxOffsetX),
      hitboxOffsetY(config.hitboxOffsetY)
{
    faction = spawner ? spawner->getFaction() : EntityFaction::Neutral;
    
    // Load animation texture if specified
    if (!config.textureName.empty()) {
        auto& assetMgr = AssetManager::getInstance();
        const Texture2D& tex = assetMgr.getTexture(config.textureName);
        if (tex.id != 0) {
            animation = std::make_unique<Animation>(tex, config.frameNum, config.frameTime, config.scale);
        }
    }

    // Phân tích sprite sheet NGAY TẠI ĐÂY chứ không phải trong render().
    // Constructor chạy ở pha Process (lúc rút hàng đợi spawn), ngoài cặp
    // BeginDrawing/EndDrawing — đọc ngược texture từ GPU ở đó an toàn hơn hẳn
    // so với làm giữa lúc đang vẽ. Kết quả có cache nên chỉ tốn một lần cho
    // mỗi sprite sheet trong cả phiên chơi.
    if (alignFramesByContent && animation) {
        frameCenters_ = GetFrameContentCenters(textureName, animation->getTexture(),
                                               animation->getFrameNum());
    }

    float autoW = config.hitboxW;
    float autoH = config.hitboxH;
    if (autoW <= 0.0f || autoH <= 0.0f) {
        if (animation) {
            Rectangle frame = animation->getCurrentFrame();
            autoW = std::abs(frame.width) * config.scale;
            autoH = std::abs(frame.height) * config.scale;
        } else {
            autoW = 16.0f;
            autoH = 16.0f;
        }
        baseStats.physicsBox = {autoW, autoH};
        runtimeStats.physicsBox = {autoW, autoH};
    }

    std::cout << "[Fireball] Created at (" << startPos.x << ", " << startPos.y << ") facing " << (isFacingRight ? "right" : "left")
              << " speed=" << config.speed << " damage=" << config.damage << std::endl;
}

// =============================================================================
// Vòng đời âm thanh của viên đạn (phần của minh1).
//
// Đạn bay có tiếng riêng, và tiếng đó phải TẮT khi đạn biến mất — nếu không nó
// còn kêu sau khi quả đạn đã nổ. Đạn biến mất theo bốn đường khác nhau: hết
// thời gian sống, chạm tường, trúng mục tiêu, và bị huỷ cùng màn chơi.
//
// Dồn hết vào deactivate() rồi cho cả bốn đường đi qua đó thì không sót đường
// nào. Destructor gọi thêm một lần cho trường hợp đối tượng bị xoá khi vẫn còn
// đang hoạt động (đổi phòng, thoát màn).
// =============================================================================
Fireball::~Fireball() {
    stopSound();
}

void Fireball::stopSound() {
    if (soundKey.empty()) return;
    auto& mgr = AssetManager::getInstance();
    if (!mgr.hasSound(soundKey)) return;
    Sound s = mgr.getSound(soundKey);
    if (IsSoundPlaying(s)) {
        StopSound(s);
    }
}

void Fireball::deactivate() {
    isActive = false;
    stopSound();
}

void Fireball::update(float dt) {
    if (!isActive) return;
    
    elapsedTime += dt;
    lifetime -= dt;
    if (lifetime <= 0) {
        std::cout << "[Fireball] Expired by lifetime" << std::endl;
        deactivate();
        return;
    }

    // Apply sine-wave curve if configured
    if (curveAmplitude > 0.0f && curveFrequency > 0.0f) {
        worldStats.position.y = originY + curveAmplitude * std::sin(curveFrequency * elapsedTime);
    }

    // Update animation if available
    if (animation) {
        animation->update(dt);
    }
}

void Fireball::render(float alpha) {
    if (!isActive) return;

    // Tia sáng vẽ TRƯỚC quả đạn để quả đạn nằm đè lên đầu tia.
    if (beamFromOwner) {
        renderOwnerBeam();
    }

    if (animation) {
        // Draw animated sprite
        Rectangle source = animation->getCurrentFrame();
        if (!worldStats.isFacingRight) {
            source.width = -source.width;
        }

        float scale = animation->getScale();
        float absW = (source.width < 0 ? -source.width : source.width) * scale;
        float absH = source.height * scale;

        // Center on position, bottom-aligned
        Rectangle dest = {
            worldStats.position.x - absW / 2.0f,
            worldStats.position.y - absH,
            absW, absH
        };

        // --- Canh theo nội dung khung, CHỈ TRỤC X (phi tiêu Naruto) ---------
        // Trục Y giữ nguyên cách neo mép dưới như mọi viên đạn khác — canh cả
        // Y làm quả cầu nổ bị nhấc lên cao hơn độ cao vốn có của chiêu.
        // Chỉ trục X cần bù, vì trong vfx_1.png phi tiêu nằm lệch hẳn sang trái
        // khung (tâm 0.115) còn quả cầu nổ nằm giữa khung (tâm 0.46).
        if (frameCenters_ && !frameCenters_->empty()) {
            int idx = animation->getCurrentFrameIndex();
            if (idx < 0) idx = 0;
            if (idx >= (int)frameCenters_->size()) idx = (int)frameCenters_->size() - 1;
            const float cx = (*frameCenters_)[idx].x;

            // Quay mặt sang trái thì source.width âm -> DrawTexturePro lật ảnh
            // trong khung dest, nên nội dung ở tỉ lệ cx tính từ mép trái sẽ
            // hiện ra ở tỉ lệ (1 - cx). Không bù lại thì đổi hướng bắn là lệch.
            const float ratioX = worldStats.isFacingRight ? cx : (1.0f - cx);
            dest.x = worldStats.position.x - ratioX * absW;
        }

        DrawTexturePro(animation->getTexture(), source, dest, {0, 0}, 0.0f, WHITE);
    } else {
        // Fallback: draw a circle
        DrawCircle(
            static_cast<int>(worldStats.position.x),
            static_cast<int>(worldStats.position.y - runtimeStats.physicsBox.y / 2),
            8.0f, ORANGE
        );
    }

    // Debug: draw hitbox outline
    DrawRectangleLinesEx(getOffsetHitbox(), 1.0f, RED);
}

// =============================================================================
// Tia sáng nối người bắn với quả đạn (chưởng kamehameha của Goku).
//
// Tia KHÔNG phải là ảnh riêng — nó nằm sẵn trong sprite nhân vật
// (special_attack.png, khung 5-8) nhưng bị cắt cụt ở mép khung 128px. Ở đây ta
// lấy đúng CỘT ĐIỂM ẢNH NGOÀI CÙNG của khung đang chạy — tức mặt cắt ngang của
// đuôi tia, đủ cả lõi trắng lẫn viền xanh — rồi kéo giãn theo chiều ngang để
// nối phần còn thiếu từ mép sprite tới chỗ quả đạn.
//
// Cách này giữ nguyên màu, độ dày và độ mờ của tia theo từng khung, và tự tắt
// đúng lúc: khung nào không có tia (Goku chưa bắn, hoặc đã về idle) thì cột
// ngoài cùng trong suốt, vẽ ra không thấy gì.
// =============================================================================
void Fireball::renderOwnerBeam() const {
    if (!spawner) return;

    const Animation* ownerAnim = spawner->getWorldStats().animation;
    if (!ownerAnim) return;

    const Texture2D& tex = ownerAnim->getTexture();
    if (tex.id == 0) return;

    const Rectangle frame = ownerAnim->getCurrentFrame();
    if (frame.width <= 0.0f || frame.height <= 0.0f) return;

    // Dựng lại đúng hình chữ nhật mà Player::render dùng để vẽ nhân vật, để tia
    // nối liền mạch vào sprite thay vì lệch lên/xuống.
    const float ownerScale = ownerAnim->getScale();
    const float ownerW = frame.width  * ownerScale;
    const float ownerH = frame.height * ownerScale;
    const Vector2 ownerPos = spawner->getPosition();
    const float ownerLeft = ownerPos.x - ownerW / 2.0f;
    const float ownerTop  = ownerPos.y - ownerH;

    const bool facingRight = spawner->getWorldStats().isFacingRight;

    // Cột ngoài cùng theo hướng bắn = mặt cắt đuôi tia.
    Rectangle slice = { facingRight ? (frame.x + frame.width - 1.0f) : frame.x,
                        frame.y, 1.0f, frame.height };
    if (!facingRight) slice.width = -slice.width;   // lật cho khớp hướng nhân vật

    // Tia chạy từ mép sprite tới tâm quả đạn.
    const float beamStart = facingRight ? (ownerLeft + ownerW) : ownerLeft;
    const float beamEnd   = worldStats.position.x;
    const float length    = facingRight ? (beamEnd - beamStart) : (beamStart - beamEnd);
    if (length <= 1.0f) return;   // đạn còn nằm trong thân sprite, chưa cần nối

    const Rectangle dest = { facingRight ? beamStart : beamEnd,
                             ownerTop, length, ownerH };
    DrawTexturePro(tex, slice, dest, {0, 0}, 0.0f, WHITE);
}

void Fireball::onHitWall(bool isRightWall, bool isCliff) {
    std::cout << "[Fireball] Hit wall! Deactivating. pos=(" << worldStats.position.x << ", " << worldStats.position.y << ")" << std::endl;
    deactivate();
}

void Fireball::onCollide(Entity& other) {
    deactivate();
}

bool Fireball::hasActiveHitbox() const {
    return isActive;
}

Rectangle Fireball::getOffsetHitbox() const {
    Rectangle baseHitbox = getHitbox();
    float offX = worldStats.isFacingRight ? hitboxOffsetX : -hitboxOffsetX;
    baseHitbox.x += offX;
    baseHitbox.y += hitboxOffsetY;
    return baseHitbox;
}

Hitbox Fireball::getActiveHitbox() {
    Rectangle rect = getOffsetHitbox();
    Hitbox hb = { rect, attackPower, 0, this, spawner };
    if (faction == EntityFaction::Player) {
        hb.targetFactionMask = (1 << static_cast<int>(EntityFaction::Enemy)) | (1 << static_cast<int>(EntityFaction::Environment));
        if (spawner && spawner->isPvPEnabled()) {
            hb.targetFactionMask |= (1 << static_cast<int>(EntityFaction::Player));
        }
    } else if (faction == EntityFaction::Enemy) {
        hb.targetFactionMask = (1 << static_cast<int>(EntityFaction::Player)) | (1 << static_cast<int>(EntityFaction::Environment));
    }
    return hb;
}

void Fireball::takeDamage(int damage, float knockbackDirX, bool forceInterrupt) {
    // Fireball is destroyed when it takes any damage (e.g. fireball-vs-fireball cancel)
    deactivate();
}
