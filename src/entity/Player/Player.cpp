#include "Player.h"
#include "AssetManager.h"
#include "SettingsManager.h"
#include "ISkill.h"
#include "PlayerCommands.h"
#include "SpawnCommand.h"
#include "raylib.h"
#include "BaseItem.h"
#include "ItemUsageFactory.h"
#include "Effects.h"
#include <cmath>
#include <iostream>

Player::Player(CharacterBaseStats &bS, CharacterRuntimeStats &rS,
               CharacterWorldStats &wS,
               std::unordered_map<std::string, Animation> animations)
    : Entity(bS, rS, wS), idleState(*this), runState(*this), jumpState(*this),
      fallState(*this), crouchState(*this), hurtState(*this), dieState(*this),
      skillState(*this), swimState(*this), climbState(*this),
      animationList(std::move(animations)) {
  currentState = &idleState;
  currentState->onEnter();
  faction = EntityFaction::Player;
}

// =============================================================================
// HIT-STOP — khựng khung hình đúng khoảnh khắc đòn chạm.
//
// Khi một đòn ăn vào mục tiêu, animation và đồng hồ chiêu của NGƯỜI ĐÁNH đứng
// yên trong vài phần trăm giây. Cú đánh nhờ vậy có "sức nặng": trúng và hụt
// trông khác hẳn nhau dù dùng chung một animation.
//
// Thời lượng nằm sẵn trong từng chiêu (hitStopDuration, nạp từ characters.json)
// nên chỉnh cảm giác đánh là sửa JSON chứ không phải sửa code.
//
// LƯU Ý: trường hitStopDuration đã có từ lâu và vẫn được nạp đầy đủ từ JSON,
// nhưng KHÔNG một chỗ nào gọi getHitStopDuration() — nó là dữ liệu chết. Cơ chế
// dưới đây mới là chỗ biến nó thành hiệu ứng thật.
// =============================================================================
void Player::onDealtDamage(Entity* target, int amount) {
  (void)target;
  (void)amount;
  hitLandedThisStep_ = true;
  if (currentState != &skillState) return;

  // CHỈ MỘT LẦN cho mỗi lần ra chiêu.
  //
  // LỖI ĐÃ SỬA — đòn đánh bị kéo dài ra trông thấy khi trúng quái.
  // Trước đây mỗi lần đòn chạm là gán lại hitStopTimer_. Nghe thì vô hại,
  // nhưng nó cộng dồn theo một vòng khép kín:
  //
  //   đóng băng -> currentState->update() bị bỏ qua -> đồng hồ chiêu đứng yên
  //   -> getElapsedTime() không đổi -> isHitboxActive() vẫn true ở đúng khung đó
  //   -> hitbox còn sống, con quái tiếp theo bước vào là lại trúng
  //   -> nạp thêm 0.05s nữa...
  //
  // Đánh một con thì chỉ dài thêm 0.05s, khó thấy. Nhưng đánh giữa đám đông thì
  // cộng lại thành vài phần mười giây và animation lê ra rất rõ.
  //
  // Hit-stop đúng nghĩa là MỘT nhịp khựng cho MỘT cú đánh, không phải một nhịp
  // cho mỗi mục tiêu. Khoá lại sau lần đầu thì độ dài chiêu bị đội thêm nhiều
  // nhất đúng một hitStopDuration, bất kể trúng bao nhiêu con.
  if (hitStopUsedThisSkill_) return;

  if (const ISkill* skill = skillState.getCurrentSkill()) {
    hitStopTimer_ = skill->getHitStopDuration();
    hitStopUsedThisSkill_ = true;
  }
}

void Player::update(float dt) {
  if (runtimeStats.iframeTimer > 0.0f) {
    runtimeStats.iframeTimer -= dt;
  }
  if (runtimeStats.disableInputTimer > 0.0f) {
    runtimeStats.disableInputTimer -= dt;
  }

  updateEffects(dt);
  overlappingItem_ = nullptr; // Reset each frame; collision loop in GameState will set it if still overlapping

  // Đang khựng vì vừa đánh trúng: bỏ qua state và animation lượt này.
  //
  // Chỉ đóng băng HÌNH ẢNH và đồng hồ chiêu. Vật lý nằm ở updatePhysicsWithMap
  // do level gọi riêng nên vẫn chạy — nhân vật đang bay không bị treo lơ lửng.
  // Buff và chữ nổi cũng chạy tiếp, vì đóng băng cả chúng thì đồng hồ buff sẽ
  // trôi chậm dần mỗi lần người chơi đánh trúng.
  if (hitStopTimer_ > 0.0f) {
    hitStopTimer_ -= dt;
    buffManager_.update(dt, *this);
    updateFloatingTexts(dt);
    return;
  }

  // Hồi mana ở MỌI trạng thái (chạy, nhảy, bơi, leo, đang ra chiêu...).
  // Trước đây lời gọi này nằm trong PlayerIdleState::update nên chỉ đứng yên
  // mới hồi. Chết thì thôi — hồi sinh sẽ nạp đầy lại trong onDie().
  if (currentState != &dieState) {
    increaseMana(kManaRegenPerSecond * dt);
  }

  if (currentState) {
    currentState->update(dt);
  }
  processBreath(dt);
  if (worldStats.animation) {
    worldStats.animation->update(dt);
  }
  updateSound();
  buffManager_.update(dt, *this);
  updateFloatingTexts(dt);
}

void Player::render(float alpha) {
  if (!worldStats.animation)
    return;

  // Blink effect during invincibility: skip rendering on alternating intervals
  if (runtimeStats.iframeTimer > 0.0f) {
    // Blink at ~10 Hz: hidden when sin > 0, visible when sin < 0
    float blinkPhase = std::sin(runtimeStats.iframeTimer * 20.0f * 3.14159f);
    if (blinkPhase > 0.0f)
      return;
  }

  Rectangle source = worldStats.animation->getCurrentFrame();
  if (!worldStats.isFacingRight) {
    source.width = -source.width;
  }

  float scale = worldStats.animation->getScale();
  float absW = (source.width < 0 ? -source.width : source.width) * scale;
  float absH = source.height * scale;

  // Neo hình ảnh bottom-center theo vị trí physics
  Rectangle dest = {worldStats.position.x - (absW / 2.0f),
                    worldStats.position.y - absH, absW, absH};

  Color tint = (runtimeStats.iframeTimer > 0.0f) ? RED : WHITE;

  DrawTexturePro(worldStats.animation->getTexture(), source, dest, {0, 0},
                 0.0f, tint);

  // Debug hitbox
  DrawRectangleLinesEx(getHitbox(), 1.0f, RED);
  for (auto& eff : activeEffects) {
      eff->render(*this, alpha);
  }
  
  buffManager_.render(*this, alpha);
  
  renderFloatingTexts();
}

void Player::changeState(PlayerState &state) {
  if (currentState == &state)
    return;
  if (currentState) {
    currentState->onExit();
  }
  currentState = &state;
  currentState->onEnter();
}

void Player::requestState(PlayerState &state) {
  if (currentState && !currentState->canExit())
    return;
  changeState(state);
}

void Player::forceState(PlayerState &state) {
  changeState(state);
}

bool Player::isDead() const {
  return runtimeStats.health <= 0;
}

bool Player::isOutOfBounds(float limitY) const {
  return worldStats.position.y > limitY;
}

void Player::respawn(Vector2 startPos) {
  worldStats.position = startPos;
  runtimeStats.velocity = {0.0f, 0.0f};
  runtimeStats.health = baseStats.maxHealth;
  runtimeStats.breath = baseStats.maxBreath;
  clearEffects();
  forceState(idleState);
}

// =============================================================================
// SERIALIZATION — Player tự đóng gói và tự khôi phục chính mình.
//
// NGUYÊN TẮC "Tell, Don't Ask": trước đây BaseLevelState phải thò tay vào
// getRuntimeStatsMutable() / getBaseStatsMutable() để moi ra rồi nhét lại từng
// trường một. Đó là phá vỡ đóng gói: mỗi lần Player thêm một thuộc tính là
// BaseLevelState phải sửa theo. Nay Player là nơi duy nhất biết mình gồm những
// gì, còn BaseLevelState chỉ việc uỷ quyền.
// =============================================================================

PlayerSaveData Player::createSaveData() const {
  PlayerSaveData data;
  data.exists = true;

  // baseStats.name chính là nhân vật đã chọn ở màn Character Selection
  // ("Goku"/"Naruto"/...). Không có nó thì khi Load Game không dựng lại nổi
  // đúng Player — PlayerFactory tra tên này trong assets/config/characters.json.
  data.characterName = baseStats.name;

  data.posX = worldStats.position.x;
  data.posY = worldStats.position.y;
  data.isFacingRight = worldStats.isFacingRight;

  data.health    = runtimeStats.health;
  data.maxHealth = baseStats.maxHealth;
  data.mana      = runtimeStats.mana;
  data.maxMana   = baseStats.maxMana;
  data.breath    = runtimeStats.breath;

  data.storedItemSlot = runtimeStats.storedItemSlot;
  return data;
}

void Player::restoreFromSaveData(const PlayerSaveData &data) {
  if (!data.exists) return;

  setPosition({data.posX, data.posY});
  worldStats.isFacingRight = data.isFacingRight;

  // maxHealth/maxMana đến từ characters.json khi dựng Player, nhưng vẫn khôi
  // phục từ save để phòng trường hợp file config được chỉnh sau khi lưu — máu
  // hiện tại phải luôn nằm trong khoảng hợp lệ của bản lưu đó.
  if (data.maxHealth > 0) baseStats.maxHealth = data.maxHealth;
  if (data.maxMana   > 0) baseStats.maxMana   = data.maxMana;

  runtimeStats.health = data.health;
  runtimeStats.mana   = data.mana;
  if (data.breath > 0) runtimeStats.breath = data.breath;

  runtimeStats.storedItemSlot = data.storedItemSlot;

  // Vận tốc không được lưu: người chơi luôn xuất hiện lại ở trạng thái đứng yên
  // thay vì đang bay giữa chừng như lúc bấm lưu.
  runtimeStats.velocity = {0.0f, 0.0f};
}

// =============================================================================
// SKILL SYSTEM
// =============================================================================

void Player::useSkill(const std::string &skillname) {
  auto it = skillList.find(skillname);
  if (it != skillList.end() && it->second) {
    ISkill *skill = it->second.get();
    if (runtimeStats.mana < skill->getManaCost()) {
      addFloatingText("Not enough mana", BLUE, {0, 0}, 0.3);
      return;
    }
    
    // Prevent interrupting a skill with another skill (or the same one)
    if (currentState == &skillState) {
        if (skillState.getCurrentSkill() == skill && skillname == "Block") {
            skillState.resetTimer();
        }
        return;
    }

    skillState.setSkill(skill);
    changeState(skillState);
  }
}

void Player::stopSkill(const std::string &skillname) {
  if (currentState == &skillState) {
    const ISkill *current = skillState.getCurrentSkill();
    if (current && current == findSkill(skillname)) {
        skillState.forceStop();
    }
  }
}

void Player::addSkill(const std::string &name, std::unique_ptr<ISkill> skill) {
  skillList[name] = std::move(skill);
}

ISkill *Player::findSkill(const std::string &skillName) {
  auto it = skillList.find(skillName);
  if (it != skillList.end())
    return it->second.get();
  return nullptr;
}

bool Player::hasEnoughMana(float cost) const {
  return runtimeStats.mana >= static_cast<int>(cost);
}

// =============================================================================
// INPUT DISPATCHERS
// =============================================================================

void Player::onMoveRight() {
  if (currentState) currentState->onMoveRight();
}

void Player::onMoveLeft() {
  if (currentState) currentState->onMoveLeft();
}

void Player::onJump() {
  if (currentState) currentState->onJump();
}

void Player::onStopLeft() {
  if (currentState) currentState->onStopLeft();
}

void Player::onStopRight() {
  if (currentState) currentState->onStopRight();
}

void Player::onCrouch() {
  // OneWay drop-through: triggered first, independent of state
  dropThrough();

  // Ladder climb-down: if overlapping ladder and not already climbing, enter ClimbState
  if (runtimeStats.isOverlappingLadder && currentState != &climbState) {
    requestState(climbState);
  }

  if (currentState) currentState->onCrouch();
}

void Player::onStopCrouch() {
  if (currentState) currentState->onStopCrouch();
}

void Player::onAttack() {
  if (currentState) currentState->onAttack();
}

void Player::onClimb() {
  // Enter climb state if on ladder
  if (runtimeStats.isOverlappingLadder && currentState != &climbState) {
    requestState(climbState);
  }
  if (currentState) currentState->onClimb();
}

// =============================================================================
// MOVEMENT HELPERS (called by States — Tell, Don't Ask)
// =============================================================================

void Player::playSound(const std::string& soundKey, bool loop) {
    if (currentAnimLooping && !currentSoundKey.empty() && AssetManager::getInstance().hasSound(currentSoundKey)) {
        StopSound(AssetManager::getInstance().getSound(currentSoundKey));
    }
    
    currentSoundKey = soundKey;
    currentAnimLooping = loop;
    
    if (AssetManager::getInstance().hasSound(currentSoundKey)) {
        Sound s = AssetManager::getInstance().getSound(currentSoundKey);
        SetSoundVolume(s, SettingsManager::GetInstance().GetPlayerSFXVolume());
        
        if (currentSoundKey.find("idle") != std::string::npos) {
            if (idleSoundTimer >= 300.0f) {
                PlaySound(s);
                idleSoundTimer = 0.0f;
            }
        } else {
            PlaySound(s);
        }
    }
}

void Player::updateSound() {
    float finalVolume = SettingsManager::GetInstance().GetPlayerSFXVolume();

    // Handle frame-based sound events
    if (soundFrames.find(currentBaseAnimName) != soundFrames.end()) {
        int currentFrameIndex = -1;
        if (worldStats.animation) {
            currentFrameIndex = worldStats.animation->getCurrentFrameIndex();
        }
        
        if (currentFrameIndex != lastSoundFrameIndex && currentFrameIndex != -1) {
            std::cout << "[DEBUG] Frame-based Sound Triggered - Anim: " << currentBaseAnimName 
                      << ", Frame: " << currentFrameIndex << std::endl;
            
            lastSoundFrameIndex = currentFrameIndex;
            auto& framesMap = soundFrames[currentBaseAnimName];
            if (framesMap.find(currentFrameIndex) != framesMap.end()) {
                std::string soundSuffix = framesMap[currentFrameIndex];
                std::string soundKey = baseStats.name + "_" + soundSuffix + "_sound";
                
                std::cout << "[DEBUG] Try playing: " << soundKey << std::endl;
                
                if (AssetManager::getInstance().hasSound(soundKey)) {
                    std::cout << "[DEBUG] Sound " << soundKey << " IS loaded! Playing..." << std::endl;
                    Sound s = AssetManager::getInstance().getSound(soundKey);
                    SetSoundVolume(s, finalVolume);
                    PlaySound(s);
                } else {
                    std::cout << "[ERROR] Sound " << soundKey << " is NOT loaded in AssetManager!" << std::endl;
                }
            }
        }
        return; // Skip generic logic if this animation uses frame-based sounds
    }


    idleSoundTimer += GetFrameTime();

    if (!currentSoundKey.empty() && AssetManager::getInstance().hasSound(currentSoundKey)) {
        Sound s = AssetManager::getInstance().getSound(currentSoundKey);
        
        if (currentSoundKey.find("idle") != std::string::npos) {
            if (idleSoundTimer >= 300.0f) {
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

const Animation *Player::findAnimation(const std::string &name) const {
  auto it = animationList.find(name);
  return (it != animationList.end()) ? &it->second : nullptr;
}

void Player::playAnimation(const std::string &name, bool loop) {
  auto it = animationList.find(name);
  if (it != animationList.end()) {
    worldStats.animation = &it->second;
    worldStats.animation->resetAnimation();
    worldStats.animation->setLoop(loop);
    
    currentBaseAnimName = name;
    lastSoundFrameIndex = -1;
    
    bool soundLoop = loop;
    if (name.find("attack") != std::string::npos ||
        name.find("hurt") != std::string::npos ||
        name.find("die") != std::string::npos ||
        name.find("special") != std::string::npos ||
        name.find("idle") != std::string::npos) {
        soundLoop = false;
    }
    
    if (soundFrames.find(name) != soundFrames.end()) {
        currentSoundKey = baseStats.name + "_" + name;
        currentAnimLooping = false;
        return;
    }
    
    playSound(baseStats.name + "_" + name + "_sound", soundLoop);
  }
}

void Player::moveRight() {
  if (runtimeStats.disableInputTimer > 0.0f) return;
  worldStats.isFacingRight = true;
  float mod = 1.0f;
  if (runtimeStats.currentLiquid == CollisionType::Poison || runtimeStats.currentLiquid == CollisionType::Lava) mod = 0.5f;
  else if (runtimeStats.currentLiquid == CollisionType::Water) mod = 0.7f;
  runtimeStats.velocity.x = baseStats.moveVelocity * (1.0f + buffManager_.getTotalSpeedMultiplier()) * mod;
}

void Player::moveLeft() {
  if (runtimeStats.disableInputTimer > 0.0f) return;
  worldStats.isFacingRight = false;
  float mod = 1.0f;
  if (runtimeStats.currentLiquid == CollisionType::Poison || runtimeStats.currentLiquid == CollisionType::Lava) mod = 0.5f;
  else if (runtimeStats.currentLiquid == CollisionType::Water) mod = 0.7f;
  runtimeStats.velocity.x = -baseStats.moveVelocity * (1.0f + buffManager_.getTotalSpeedMultiplier()) * mod;
}

void Player::stopLeftRun() {
  if (runtimeStats.disableInputTimer > 0.0f) return;
  if (runtimeStats.velocity.x < 0.0f)
    runtimeStats.velocity.x = 0.0f;
}

void Player::stopRightRun() {
  if (runtimeStats.disableInputTimer > 0.0f) return;
  if (runtimeStats.velocity.x > 0.0f)
    runtimeStats.velocity.x = 0.0f;
}

void Player::jump() { 
  float mod = 1.0f;
  if (runtimeStats.currentLiquid == CollisionType::Poison || runtimeStats.currentLiquid == CollisionType::Lava) mod = 0.6f;
  else if (runtimeStats.currentLiquid == CollisionType::Water) mod = 0.8f;
  runtimeStats.velocity.y = baseStats.jumpVelocity * (1.0f + buffManager_.getTotalJumpMultiplier()) * mod; 
}

void Player::crouch() {
  // Resize hitbox to crouch dimensions; stop horizontal movement
  runtimeStats.physicsBox = baseStats.crouchBox;
  runtimeStats.velocity.x = 0.0f;
}

void Player::standUp() {
  // Restore original physics hitbox
  runtimeStats.physicsBox = baseStats.physicsBox;
}

void Player::speedUpX(float speedX) {
  runtimeStats.velocity.x = worldStats.isFacingRight ? speedX : -speedX;
}

void Player::speedUpY(float speedY) {
  runtimeStats.velocity.y = speedY;
}

void Player::idle() {
  runtimeStats.physicsBox = baseStats.physicsBox;
  runtimeStats.velocity = {0.0f, 0.0f};
}

void Player::reduceMana(float cost) {
  runtimeStats.mana -= static_cast<int>(cost);
  if (runtimeStats.mana < 0)
    runtimeStats.mana = 0;
}

void Player::increaseMana(float cost) {
  runtimeStats.manaAccumulator += cost;
  if (runtimeStats.manaAccumulator >= 1.0f) {
    int manaToAdd = static_cast<int>(runtimeStats.manaAccumulator);
    runtimeStats.mana += manaToAdd;
    runtimeStats.manaAccumulator -= manaToAdd;
    if (runtimeStats.mana > baseStats.maxMana)
      runtimeStats.mana = baseStats.maxMana;
  }
}

// --- Swim & Climb helpers ---

void Player::swim(float dirX) {
  float speedMod = 1.0f;
  if (runtimeStats.currentLiquid == CollisionType::Water) speedMod = 0.7f;
  else if (runtimeStats.currentLiquid == CollisionType::Poison || runtimeStats.currentLiquid == CollisionType::Lava) speedMod = 0.4f;

  const float swimSpeed = baseStats.moveVelocity * speedMod;
  if (dirX > 0.0f) {
    worldStats.isFacingRight = true;
    runtimeStats.velocity.x = swimSpeed;
  } else if (dirX < 0.0f) {
    worldStats.isFacingRight = false;
    runtimeStats.velocity.x = -swimSpeed;
  } else {
    runtimeStats.velocity.x = 0.0f;
  }
}

void Player::swimY(float dirY) {
  float speedMod = 1.0f;
  if (runtimeStats.currentLiquid == CollisionType::Water) speedMod = 0.7f;
  else if (runtimeStats.currentLiquid == CollisionType::Poison || runtimeStats.currentLiquid == CollisionType::Lava) speedMod = 0.4f;

  const float swimSpeed = baseStats.moveVelocity * speedMod;
  if (dirY > 0.0f) {
    runtimeStats.velocity.y = swimSpeed;
  } else if (dirY < 0.0f) {
    runtimeStats.velocity.y = -swimSpeed;
  } else {
    runtimeStats.velocity.y = 0.0f;
  }
}

void Player::climbUp() {
  runtimeStats.velocity.y = -baseStats.moveVelocity * 0.5f;
  runtimeStats.velocity.x = 0.0f;
}

void Player::climbDown() {
  runtimeStats.velocity.y = baseStats.moveVelocity * 0.5f;
  runtimeStats.velocity.x = 0.0f;
}

void Player::stopClimb() {
  runtimeStats.velocity.y = 0.0f;
  runtimeStats.velocity.x = 0.0f;
}

// =============================================================================
// COMBAT
// =============================================================================

bool Player::hasActiveHitbox() const {
  return currentState == &skillState && skillState.isHitboxActive();
}

Hitbox Player::getActiveHitbox() {
  const ISkill *skill = skillState.getCurrentSkill();
  Rectangle box = skill->getHitboxConfig();
  float offsetX = worldStats.isFacingRight ? box.x : -box.x;
  Rectangle worldRect = {
      worldStats.position.x + offsetX - box.width / 2.0f,
      worldStats.position.y - runtimeStats.physicsBox.y + box.y, box.width,
      box.height};
  // Clean: no const_cast — getActiveHitbox() is non-const
  Hitbox hb = {worldRect, skill->getAttackPower(), skill->getDefensePower(), this};
  hb.targetFactionMask = (1 << static_cast<int>(EntityFaction::Enemy)) | (1 << static_cast<int>(EntityFaction::Environment));
  if (isPvPMode_) {
      hb.targetFactionMask |= (1 << static_cast<int>(EntityFaction::Player));
  }
  return hb;
}

void Player::takeDamage(int damage, float knockbackDirX, bool forceInterrupt) {
  if (currentState == &dieState || buffManager_.isInvincible())
    return;

  if (forceInterrupt) {
    if (currentState == &hurtState || runtimeStats.iframeTimer > 0.0f)
      return;
      
    runtimeStats.health -= damage;
    runtimeStats.iframeTimer = 1.0f; // 1 second of invincibility
    
    addFloatingText(std::to_string(damage), RED, {0, 0}, 0.5f);
    
    // Apply a small knockback upwards to break free from continuous ground hazards
    runtimeStats.velocity.y = -200.0f;
    if (knockbackDirX != 0.0f) {
        runtimeStats.velocity.x = 250.0f * knockbackDirX;
    } else {
        // Default to pushing backwards if no direction provided
        runtimeStats.velocity.x = worldStats.isFacingRight ? -250.0f : 250.0f;
    }

    if (runtimeStats.health <= 0) {
      changeState(dieState);
    } else {
      changeState(hurtState);
    }
  } else {
    // DOT damage ignores iframeTimer and hurtState invincibility, but still deals damage
    runtimeStats.health -= damage;
    addFloatingText(std::to_string(damage), RED, {0, 0}, 0.5f);

    if (runtimeStats.health <= 0) {
      changeState(dieState);
    } else {
      if (currentState != &hurtState) {
        changeState(hurtState);
      }
    }
  }
}

// =============================================================================
// PHYSICS HOOKS
// =============================================================================

void Player::onLand(float floorY) {
  // isGrounded is already set true by updatePhysicsWithMap
}

void Player::onHitCeiling(float ceilY) {
  // Reserved for brick-breaking or sound effects
}

void Player::onEnterWater() { 
  if (currentState == &swimState) return;
  // Don't interrupt a jump out of the water if the player's head is still above water
  if (runtimeStats.isPartiallyOutsideLiquid && currentState == &jumpState) {
      return;
  }
  requestState(swimState); 
}
void Player::onExitLiquid() { 
  if (currentState == &swimState) {
      requestState(fallState);
  }
}

void Player::onOverlapLadder() {
  // Auto-enter climb only if player explicitly presses climb key (onClimb),
  // not auto. Leave this hook for audio/visual feedback only.
}

// Ô gạch giết ngay (Die/Hazard). Phải dùng forceState chứ KHÔNG phải
// requestState: crouchState, hurtState và skillState đều trả canExit()==false,
// nên requestState bị nuốt lặng lẽ — ngồi xổm hoặc đang ra chiêu mà rơi vào ô
// chết thì không chết, đứng nguyên đó bất tử.
void Player::onHazard() {
  if (currentState == &dieState) return;
  forceState(dieState);
}

void Player::onDie() { 
    Entity::onDie();
    worldStats.position = worldStats.startPosition;
    runtimeStats.velocity = {0.0f, 0.0f};
    runtimeStats.health = baseStats.maxHealth;
    runtimeStats.mana = baseStats.maxMana;
    runtimeStats.breath = baseStats.maxBreath;
    clearEffects();
    buffManager_.clear(*this);
    // dieState.canExit() == false, nên requestState ở đây không bao giờ thành
    // công: người chơi hồi sinh xong vẫn kẹt trong trạng thái chết.
    forceState(idleState);
}

void Player::updateStateFromPhysics() {
  // Do NOT interfere with self-managing states
  if (currentState == &skillState || currentState == &swimState ||
      currentState == &climbState || currentState == &hurtState ||
      currentState == &dieState)
    return;

  // Auto-snap to ladder if falling (or at apex) onto it
  if (runtimeStats.isOverlappingLadder && runtimeStats.ignoreLadderTimer <= 0.0f) {
    if (runtimeStats.velocity.y >= 0.0f) {
      requestState(climbState);
      return;
    }
  }

  if (!runtimeStats.isGrounded) {
    if (runtimeStats.velocity.y > 0) {
      requestState(fallState);
    } else {
      requestState(jumpState);
    }
  } else {
    if (runtimeStats.velocity.x == 0.0f) {
      requestState(idleState);
    } else {
      requestState(runState);
    }
  }
}

// =============================================================================
// FIREBALL / SPAWN
// =============================================================================

void Player::spawnFireball() {
  if (!commandQueue) return;

  SpawnCommand cmd;
  cmd.type = EntityType::Fireball;
  cmd.position = worldStats.position;
  cmd.isFacingRight = worldStats.isFacingRight;
  cmd.ownerName = baseStats.name;
  cmd.spawner = this;

  commandQueue->push(cmd);
}

void Player::spawnSpecialBall() {
  if (!commandQueue) return;

  SpawnCommand cmd;
  cmd.type = EntityType::SpecialBall;
  cmd.position = worldStats.position;
  cmd.isFacingRight = worldStats.isFacingRight;
  cmd.ownerName = baseStats.name;
  cmd.spawner = this;

  commandQueue->push(cmd);
}

void Player::spawnExplosion() {
  if (!commandQueue) return;

  SpawnCommand cmd;
  cmd.category = SpawnCategory::Entity;
  cmd.type = EntityType::Explosion;
  cmd.position = worldStats.position; // Centered on the player
  cmd.isFacingRight = worldStats.isFacingRight;
  cmd.ownerName = baseStats.name;
  cmd.spawner = this;
  
  cmd.onHitEffect = [](Entity* target) {
      if (target) {
          auto burn = std::make_unique<LavaEffect>();
          burn->setInLava(false);
          target->addEffect(std::move(burn));
          std::cout << "[Explosion] Damage and Burn applied to " << target->getBaseStats().name << "!\n";
      }
  };

  commandQueue->push(cmd);
}

void Player::interactWithOverlapping() {
    if (overlappingItem_) {
        // Standing on/near an item → swap/pickup
        overlappingItem_->forceInteract(*this);
        overlappingItem_ = nullptr;
    } else {
        // Nothing nearby → use stored item
        useStoredItem();
    }
}

void Player::useStoredItem() {
    auto& slot = runtimeStats.storedItemSlot;
    if (slot.empty()) return;

    auto strategy = ItemUsageFactory::create(slot);
    if (strategy) {
        strategy->use(*this);
        slot = ""; // Clear inventory after use
    } else {
        std::cerr << "[Player] Unrecognized item in slot: " << slot << "\n";
    }
}

void Player::dropThrough() {
    runtimeStats.ignoreOneWayTimer = 0.2f;
}

void Player::onCutsceneStart(const std::string& triggerId) {
    // We do nothing here immediately. 
    // BaseLevelState will gently stop the player only when they are on the ground,
    // allowing them to complete their jump arcs if triggered mid-air.
}

void Player::processBreath(float dt) {
    if (runtimeStats.currentLiquid == CollisionType::Water && !runtimeStats.isPartiallyOutsideLiquid) {
        // Lose 100 breath over 10 seconds -> 10 breath per second -> 1 breath per 0.1s
        runtimeStats.breathAccumulator += dt;
        if (runtimeStats.breathAccumulator >= 0.1f) {
            int ticks = static_cast<int>(runtimeStats.breathAccumulator / 0.1f);
            runtimeStats.breath -= ticks;
            runtimeStats.breathAccumulator -= ticks * 0.1f;
            if (runtimeStats.breath < 0) {
                runtimeStats.breath = 0;
            }
        }

        // Drowning damage: 10 damage every 1 second when breath is 0
        if (runtimeStats.breath == 0) {
            runtimeStats.drownDamageTimer += dt;
            if (runtimeStats.drownDamageTimer >= 1.0f) {
                takeDamage(10, 0.0f, false);
                runtimeStats.drownDamageTimer -= 1.0f;
            }
        }
    } else {
        // Recover breath: 100 breath over 2.5 seconds -> 40 breath per second -> 1 breath per 0.025s
        if (runtimeStats.breath < baseStats.maxBreath) {
            runtimeStats.breathAccumulator += dt;
            if (runtimeStats.breathAccumulator >= 0.025f) {
                int ticks = static_cast<int>(runtimeStats.breathAccumulator / 0.025f);
                runtimeStats.breath += ticks;
                runtimeStats.breathAccumulator -= ticks * 0.025f;
                if (runtimeStats.breath > baseStats.maxBreath) {
                    runtimeStats.breath = baseStats.maxBreath;
                }
            }
        } else {
            runtimeStats.breathAccumulator = 0.0f;
        }
        runtimeStats.drownDamageTimer = 0.0f;
    }
}