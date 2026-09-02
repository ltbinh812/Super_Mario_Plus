#include "Mob.h"
#include "IMobState.h"
#include "EnemyStates/EnemyHurtState.h"
#include "EnemyStates/EnemyDieState.h"
#include "Player.h"
#include "CommandQueue.h"
#include "TileMap.h"
#include "SettingsManager.h"
#include "infrastructure/AssetManager.h"
#include <iostream>
#include <cmath>
#include <raymath.h>

Mob::Mob(Vector2 worldPos, const std::string& type, const CharacterBaseStats& bStats, const MobConfig& cfg)
    : Entity(bStats, CharacterRuntimeStats(), CharacterWorldStats()),
      mobType(type), config(cfg), currentAnim(nullptr), stateTimer(0.0f),
      spawnPoint(worldPos), isFacingRight(false), isDead(false), hurtTimer(0.0f), deadTimer(0.0f)
{
    faction = EntityFaction::Enemy;
    worldStats.position = worldPos;
    runtimeStats.health = bStats.maxHealth;
    runtimeStats.physicsBox = bStats.physicsBox;
    baseStats.avoidCliffsAndWater = true; // Mob/Boss avoids falling off cliffs and entering water
    TraceLog(LOG_INFO, "[Mob] Created %s at (%f,%f) with health: %d", mobType.c_str(), worldPos.x, worldPos.y, runtimeStats.health);
}

Mob::~Mob() {
    if (currentState) {
        currentState->exit(*this);
    }
}

void Mob::update(float dt) {
    if (aggroCooldown > 0.0f) {
        aggroCooldown -= dt;
    }
    if (attackCooldown > 0.0f) {
        attackCooldown -= dt;
    }

    if (isDead) {
        bool animFinished = false;
        if (hasStandardAnimations() && currentStandardAnim) {
            currentStandardAnim->update(dt);
            animFinished = currentStandardAnim->isFinished();
        } else if (currentAnim) {
            currentAnim->update(dt);
            animFinished = currentAnim->isFinished();
        }
        
        if (animFinished) {
            deadTimer += dt;
        }
        return;
    }

    if (hurtTimer > 0.0f) {
        hurtTimer -= dt;
    }

    // === Khi nào quái được phép xuống nước ===
    //
    // avoidCliffsAndWater giữ quái khỏi đi lộn cổ xuống vực — nhưng nó coi mép
    // nước cũng là vực, nên quái đứng trên bờ quay đầu và người chơi cứ lặn
    // xuống là an toàn tuyệt đối.
    //
    // Nới ra đúng hai trường hợp:
    //   - Quái đã ở trong nước: né nữa thì vô nghĩa, chỉ khiến nó giãy giụa.
    //   - Mục tiêu đang ở trong nước: cho phép quái lội xuống mà đuổi.
    // Ngoài hai trường hợp đó, cảm biến vực vẫn bật như cũ.
    bool allowWater = isInLiquid();
    if (!allowWater) {
        if (Player* t = getClosestPlayer()) {
            auto liq = t->getRuntimeStats().currentLiquid;
            allowWater = (liq == CollisionType::Water ||
                          liq == CollisionType::Poison ||
                          liq == CollisionType::Lava);
        }
    }
    baseStats.avoidCliffsAndWater = !allowWater;

    if (currentStandardAnim) {
        currentStandardAnim->update(dt);
    } else if (currentAnim) {
        currentAnim->update(dt);
    }
    
    updateSound();
    
    // Debug pos periodically
    static float logT = 0.0f;
    logT += dt;
    if (logT > 1.0f) {
        // TraceLog(LOG_INFO, "[Mob] %s is at (%f, %f) Active: %d", mobType.c_str(), worldStats.position.x, worldStats.position.y, getIsActive());
        logT = 0.0f;
    }
}

// =============================================================================
// Bơi về phía mục tiêu.
//
// Trên cạn quái chỉ điều khiển trục X, còn trục Y do trọng lực lo. Dưới nước
// thì cả hai trục đều phải chủ động, nếu không con quái chỉ trôi ngang ở đúng
// một độ sâu và không bao giờ với tới người chơi đang lặn sâu hơn hay nổi cao hơn.
//
// Tốc độ bơi lấy 70% tốc độ chạy — dưới nước thì chậm hơn, giống hệt cách người
// chơi bị giảm tốc khi bơi.
// =============================================================================
bool Mob::swimToward(Vector2 target) {
    if (!isInLiquid()) return false;

    const float swimSpeed = baseStats.moveVelocity * 0.7f;
    Vector2 pos = getPosition();
    float dx = target.x - pos.x;
    float dy = target.y - pos.y;

    // Ngưỡng chết để quái không rung lắc quanh mục tiêu.
    const float dead = 4.0f;

    float vx = 0.0f, vy = 0.0f;
    if (dx >  dead) { vx =  swimSpeed; isFacingRight = true;  }
    else if (dx < -dead) { vx = -swimSpeed; isFacingRight = false; }

    if (dy >  dead) vy =  swimSpeed;
    else if (dy < -dead) vy = -swimSpeed;
    else vy = 0.0f;   // ngang tầm rồi thì giữ độ sâu, khỏi chìm

    runtimeStats.velocity.x = vx;
    runtimeStats.velocity.y = vy;
    return true;
}

bool Mob::dropLootOnce(const std::string& itemIdentifier) {
    if (lootDropped_) return false;
    CommandQueue* q = getCommandQueue();
    if (!q) return false;

    lootDropped_ = true;

    SpawnCommand cmd;
    cmd.category       = SpawnCategory::Item;
    cmd.itemIdentifier = itemIdentifier;
    // Gửi vị trí chân con quái. Việc nhấc lên cho dễ nhìn (và lùi lại khi vướng
    // đá) do BaseLevelState::findFreeItemSpawn() lo.
    cmd.position       = getPosition();
    q->push(cmd);
    return true;
}

void Mob::render(float alpha) {
    // Blink effect when hurt
    if (hurtTimer > 0.0f && !isDead) {
        int blink = (int)(hurtTimer * 20.0f);
        if (blink % 2 == 0) return;
    }

    if (hasStandardAnimations() && currentStandardAnim) {
        const Texture2D& tex = currentStandardAnim->getTexture();
        if (tex.id == 0) return;

        Rectangle src = currentStandardAnim->getCurrentFrame();
        if (!isFacingRight) {
            src.width = -std::abs(src.width);
        } else {
            src.width = std::abs(src.width);
        }

        float scale = currentStandardAnim->getScale();
        float drawW = std::abs(src.width) * scale;
        float drawH = std::abs(src.height) * scale;

        Rectangle dest = {
            worldStats.position.x - drawW / 2.0f,
            worldStats.position.y - drawH,
            drawW,
            drawH
        };

        DrawTexturePro(tex, src, dest, { 0, 0 }, 0.0f, WHITE);
    } else if (currentAnim && currentAnim->isValid()) {
        const Texture2D& tex = currentAnim->getTexture();
        if (tex.id == 0) return;

        Rectangle src = currentAnim->getCurrentSourceRect();
        if (!isFacingRight) {
            src.width = -std::abs(src.width);
        } else {
            src.width = std::abs(src.width);
        }

        float drawW = std::abs(src.width);
        float drawH = std::abs(src.height);

        Rectangle dest = {
            worldStats.position.x - drawW / 2.0f,
            worldStats.position.y - drawH,
            drawW,
            drawH
        };

        DrawTexturePro(tex, src, dest, { 0, 0 }, 0.0f, WHITE);
    }

    // Draw Health Bar if not dead
    if (!isDead && baseStats.maxHealth > 0) {
        float hpPercent = (float)runtimeStats.health / baseStats.maxHealth;
        if (hpPercent < 0.0f) hpPercent = 0.0f;
        
        float barWidth = 20.0f;
        float barHeight = 4.0f;
        float barX = worldStats.position.x - barWidth / 2.0f;
        float barY = worldStats.position.y + 2.0f; // Just below the mob
        
        // Background (black)
        DrawRectangle((int)barX, (int)barY, (int)barWidth, (int)barHeight, BLACK);
        // Foreground (green)
        DrawRectangle((int)barX, (int)barY, (int)(barWidth * hpPercent), (int)barHeight, GREEN);
    }

    // Render Physics Box
    Rectangle hitbox = getHitbox();
    DrawRectangleLinesEx(hitbox, 1.0f, RED);
}

void Mob::decideAction() {
    if (isDead || hurtTimer > 0.0f) return;
    runStateCallback(&IMobState::decideAction);
}

void Mob::process() {
    if (isDead) {
        // If dead animation finished, we could mark for deletion
        return;
    }
    runStateCallback(&IMobState::process);
}

// =============================================================================
// Dịch chuyển tới một ô trống cạnh người chơi.
//
// Chỉ Boss gọi (Boss::update), khi lần đầu phát hiện người chơi trong bán kính
// 5 block. Hàm này chỉ lo phần "tìm chỗ đứng hợp lệ" — điều kiện kích hoạt do
// bên gọi quyết định.
// =============================================================================
bool Mob::tryRepositionNear(const Player& target) {
    const Vector2 targetPos = target.getPosition();
    const Rectangle myBox = getHitbox();
    const float w = myBox.width;
    const float h = myBox.height;

    // Ưu tiên xuất hiện ở phía quái đang đứng (đỡ giật hình), sau đó thử phía
    // đối diện, rồi lùi dần ra xa. Mỗi ứng viên còn được nhấc lên vài nấc để
    // tránh trường hợp chỗ ngang chân người chơi bị đặc.
    const float sideFirst = (getPosition().x <= targetPos.x) ? -1.0f : 1.0f;
    const float offsets[] = {60.0f, 90.0f, 130.0f};
    const float lifts[]   = {0.0f, -32.0f, -64.0f};

    for (float dx : offsets) {
        for (float side : {sideFirst, -sideFirst}) {
            for (float lift : lifts) {
                Vector2 candidate = { targetPos.x + side * dx, targetPos.y + lift };

                // getHitbox() neo bottom-center theo position, nên dựng khung
                // thử nghiệm y hệt cách đó để phép kiểm tra khớp với lúc chạy.
                Rectangle probe = { candidate.x - w / 2.0f, candidate.y - h, w, h };

                bool blocked = false;
                if (map_) {
                    for (const auto& tile : map_->GetCollidingTiles(probe)) {
                        // Chỉ né ô đặc và ô giết ngay. Ô nước/thang/mây thì
                        // đứng được, không cần loại.
                        if (tile.type == CollisionType::Solid ||
                            tile.type == CollisionType::Die ||
                            tile.type == CollisionType::Hazard ||
                            tile.type == CollisionType::Lava) {
                            blocked = true;
                            break;
                        }
                    }
                } else {
                    // Không có map thì không dám dịch chuyển: thà để quái đứng
                    // yên còn hơn nhét nó vào trong tường.
                    return false;
                }

                if (!blocked) {
                    setPosition(candidate);
                    runtimeStats.velocity = {0.0f, 0.0f};
                    setFacingRight(side < 0.0f);
                    addFloatingText("!", ORANGE, {0, -10}, 0.6f);
                    return true;
                }
            }
        }
    }
    return false;
}

Player* Mob::getClosestPlayer() const {
    Player* closest = nullptr;
    float minDistance = 999999.0f;
    Vector2 myPos = getPosition();

    for (Player* p : targetPlayers) {
        if (p && !p->isDead() && !p->getBuffManager().isInvisible()) {
            float dist = Vector2Distance(myPos, p->getPosition());
            if (dist < minDistance) {
                minDistance = dist;
                closest = p;
            }
        }
    }
    return closest;
}

void Mob::takeDamage(int damage, float knockbackDirX, bool forceInterrupt) {
    if (isDead || hurtTimer > 0.0f) return;

    runtimeStats.health -= damage;

    if (runtimeStats.health <= 0) {
        runtimeStats.health = 0;
        isDead = true;
        changeState(std::make_unique<EnemyDieState>());
    } else if (forceInterrupt) {
        changeState(std::make_unique<EnemyHurtState>(knockbackDirX, 0.5f));
    }
}

bool Mob::getIsActive() const {
    if (!isDead) return true;
    
    // Giữ xác lại trên màn hình thêm 1.5 giây sau khi chạy xong animation die
    if (deadTimer > 1.5f) {
        return false;
    }
    
    return true;
}

void Mob::changeState(std::unique_ptr<IMobState> newState) {
    // Đang chạy trong enter/exit/decideAction/process của một trạng thái thì
    // KHÔNG được huỷ trạng thái đó ngay — chỉ ghi nhận, để runStateCallback()
    // áp dụng sau khi hàm kia đã trở về.
    if (inStateCallback_) {
        pendingState_ = std::move(newState);
        return;
    }
    applyStateNow(std::move(newState));
}

void Mob::applyStateNow(std::unique_ptr<IMobState> next) {
    // Vòng lặp vì enter() của trạng thái mới có quyền yêu cầu chuyển tiếp ngay
    // (BossAttackState::enter chọn ngẫu nhiên một skill rồi vào BossSkillState).
    // Chặn ở 8 nấc để hai trạng thái trỏ qua lại nhau không treo cả game.
    int guard = 0;
    while (next && guard++ < 8) {
        if (currentState) {
            inStateCallback_ = true;
            currentState->exit(*this);
            inStateCallback_ = false;
        }

        // Giữ trạng thái cũ SỐNG trong biến cục bộ tới hết vòng lặp: nếu huỷ
        // ngay tại đây thì phần code còn lại của hàm đang gọi (nằm trong chính
        // trạng thái đó) sẽ chạy trên bộ nhớ đã giải phóng.
        std::unique_ptr<IMobState> previous = std::move(currentState);

        currentState = std::move(next);
        stateTimer = 0.0f;

        if (currentState) {
            inStateCallback_ = true;
            currentState->enter(*this);
            inStateCallback_ = false;
        }

        next = std::move(pendingState_);
        pendingState_ = nullptr;
        // previous được huỷ ở đây — sau khi mọi hàm của nó đã trở về.
    }
}

void Mob::flushPendingState() {
    if (!pendingState_) return;
    std::unique_ptr<IMobState> next = std::move(pendingState_);
    pendingState_ = nullptr;
    applyStateNow(std::move(next));
}

void Mob::runStateCallback(void (IMobState::*fn)(Mob&)) {
    if (!currentState) return;
    inStateCallback_ = true;
    (currentState.get()->*fn)(*this);
    inStateCallback_ = false;
    flushPendingState();
}

void Mob::setAnimation(const std::string& animName) {
    if (hasStandardAnimations()) {
        auto it = standardAnimations.find(animName);
        if (it == standardAnimations.end()) {
            std::string prefix = mobType + "_";
            if (animName.find(prefix) == 0) {
                std::string base = animName.substr(prefix.length());
                it = standardAnimations.find(base);
            }
        }
        if (it != standardAnimations.end()) {
            if (currentStandardAnim != &it->second) {
                currentStandardAnim = &it->second;
                currentStandardAnim->resetAnimation();
            }
        }
    } else {
        auto it = animations.find(animName);
        if (it != animations.end()) {
            if (currentAnim != &it->second) {
                currentAnim = &it->second;
                currentAnim->reset();
            }
        }
    }
    // Extract base animation name for sound key
    std::string baseAnimName = animName;
    std::string prefix = mobType + "_";
    if (animName.find(prefix) == 0) {
        baseAnimName = animName.substr(prefix.length());
    }
    
    currentBaseAnimName = baseAnimName;

    // Play sound based on the requested animation name
    bool loop = true;
    if (currentStandardAnim) {
        loop = currentStandardAnim->isLooping();
    } else if (currentAnim) {
        loop = currentAnim->isLooping();
    }
    
    // Reset frame-based sound tracker
    lastSoundFrameIndex = -1;

    // Explicitly override looping for certain actions that should strictly be one-shots
    if (baseAnimName.find("attack") != std::string::npos ||
        baseAnimName.find("hurt") != std::string::npos ||
        baseAnimName.find("die") != std::string::npos ||
        baseAnimName.find("special") != std::string::npos ||
        baseAnimName.find("explosion") != std::string::npos ||
        baseAnimName.find("intro") != std::string::npos ||
        baseAnimName.find("idle") != std::string::npos) {
        loop = false;
    }
    
    // Disable generic looping if this animation uses frame-based sounds
    if (soundFrames.find(baseAnimName) != soundFrames.end()) {
        loop = false;
        // Don't play generic sound if frame-based mapping exists
        currentSoundKey = mobType + "_" + baseAnimName;
        currentAnimLooping = false;
        return;
    }
    
    playSound(mobType + "_" + baseAnimName + "_sound", loop);
}

void Mob::onHitWall(bool rightWall, bool isCliff) {
    if (!currentState) return;
    // Cùng lý do như decideAction/process: EnemyRunState::onHitWall gọi
    // changeState(EnemyIdleState) ngay trong thân hàm này.
    inStateCallback_ = true;
    currentState->onHitWall(*this, rightWall, isCliff);
    inStateCallback_ = false;
    flushPendingState();
}

void Mob::onLand(float floorY) {
    runtimeStats.velocity.y = 0.0f;
}

void Mob::playSound(const std::string& soundKey, bool loop) {
    if (currentAnimLooping && !currentSoundKey.empty() && AssetManager::getInstance().hasSound(currentSoundKey)) {
        StopSound(AssetManager::getInstance().getSound(currentSoundKey));
    }
    
    currentSoundKey = soundKey;
    currentAnimLooping = loop;
    
    if (AssetManager::getInstance().hasSound(currentSoundKey)) {
        Sound s = AssetManager::getInstance().getSound(currentSoundKey);
        
        float volumeModifier = 0.0f;
        Player* closest = getClosestPlayer();
        if (closest) {
            float dist = Vector2Distance(getPosition(), closest->getPosition());
            if (dist <= maxHearingDistance) {
                volumeModifier = 1.0f - (dist / maxHearingDistance);
                if (volumeModifier < 0.0f) volumeModifier = 0.0f;
            }
        }
        
        float finalVolume = SettingsManager::GetInstance().GetEnemySFXVolume() * volumeModifier;
        SetSoundVolume(s, finalVolume);
        
        if (currentSoundKey.find("idle") != std::string::npos) {
            if (idleSoundTimer >= 3.0f) {
                PlaySound(s);
                idleSoundTimer = 0.0f;
            }
        } else {
            PlaySound(s);
        }
    }
}

void Mob::updateSound() {
    float volumeModifier = 0.0f;
    Player* closest = getClosestPlayer();
    if (closest) {
        float dist = Vector2Distance(getPosition(), closest->getPosition());
        if (dist <= maxHearingDistance) {
            volumeModifier = 1.0f - (dist / maxHearingDistance);
            if (volumeModifier < 0.0f) volumeModifier = 0.0f;
        }
    }
    
    float finalVolume = SettingsManager::GetInstance().GetEnemySFXVolume() * volumeModifier;

    // Handle frame-based sound events
    if (soundFrames.find(currentBaseAnimName) != soundFrames.end()) {
        int currentFrameIndex = -1;
        if (currentStandardAnim) {
            currentFrameIndex = currentStandardAnim->getCurrentFrameIndex();
        } else if (currentAnim) {
            currentFrameIndex = currentAnim->getCurrentFrameIndex();
        }
        
        if (currentFrameIndex != lastSoundFrameIndex && currentFrameIndex != -1) {
            std::cout << "[DEBUG-MOB] Frame-based Sound Triggered - Anim: " << currentBaseAnimName 
                      << ", Frame: " << currentFrameIndex << std::endl;
            
            lastSoundFrameIndex = currentFrameIndex;
            auto& framesMap = soundFrames[currentBaseAnimName];
            if (framesMap.find(currentFrameIndex) != framesMap.end()) {
                std::string soundSuffix = framesMap[currentFrameIndex];
                std::string soundKey = mobType + "_" + soundSuffix + "_sound";
                std::cout << "[DEBUG-MOB] Try playing: " << soundKey << std::endl;
                
                if (AssetManager::getInstance().hasSound(soundKey)) {
                    std::cout << "[DEBUG-MOB] Sound " << soundKey << " IS loaded! Playing..." << std::endl;
                    Sound s = AssetManager::getInstance().getSound(soundKey);
                    SetSoundVolume(s, finalVolume);
                    PlaySound(s);
                } else {
                    std::cout << "[ERROR-MOB] Sound " << soundKey << " is NOT loaded in AssetManager!" << std::endl;
                }
            }
        }
        return; // Skip generic logic if this animation uses frame-based sounds
    }

    idleSoundTimer += GetFrameTime();

    // Handle generic sounds
    if (!currentSoundKey.empty() && AssetManager::getInstance().hasSound(currentSoundKey)) {
        Sound s = AssetManager::getInstance().getSound(currentSoundKey);
        
        if (currentSoundKey.find("idle") != std::string::npos) {
            if (idleSoundTimer >= 3.0f) {
                idleSoundTimer = 0.0f;
                SetSoundVolume(s, finalVolume);
                PlaySound(s);
            } else if (IsSoundPlaying(s)) {
                SetSoundVolume(s, finalVolume);
            }
        } else {
            if (currentAnimLooping) {
                if (!IsSoundPlaying(s)) {
                    SetSoundVolume(s, finalVolume);
                    PlaySound(s);
                } else {
                    SetSoundVolume(s, finalVolume);
                }
            } else if (IsSoundPlaying(s)) {
                // Update volume dynamically for one-shot sounds while they are playing
                SetSoundVolume(s, finalVolume);
            }
        }
    }
}
