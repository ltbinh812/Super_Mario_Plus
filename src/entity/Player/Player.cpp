#include "Player.h"
#include "AssetManager.h"
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

void Player::update(float dt) {
  if (runtimeStats.iframeTimer > 0.0f) {
    runtimeStats.iframeTimer -= dt;
  }
  
  updateEffects(dt);
  overlappingItem_ = nullptr; // Reset each frame; collision loop in GameState will set it if still overlapping
  
  if (currentState) {
    currentState->update(dt);
  }
  if (worldStats.animation) {
    worldStats.animation->update(dt);
  }
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
  clearEffects();
  forceState(idleState);
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

void Player::playAnimation(const std::string &name, bool loop) {
// std::cout << name << '\n';
  auto it = animationList.find(name);
  if (it != animationList.end()) {
    worldStats.animation = &it->second;
    worldStats.animation->resetAnimation();
    worldStats.animation->setLoop(loop);
  }
}

void Player::moveRight() {
  worldStats.isFacingRight = true;
  float mod = 1.0f;
  if (runtimeStats.currentLiquid == CollisionType::Poison || runtimeStats.currentLiquid == CollisionType::Lava) mod = 0.5f;
  else if (runtimeStats.currentLiquid == CollisionType::Water) mod = 0.7f;
  runtimeStats.velocity.x = baseStats.moveVelocity * (1.0f + buffManager_.getTotalSpeedMultiplier()) * mod;
}

void Player::moveLeft() {
  worldStats.isFacingRight = false;
  float mod = 1.0f;
  if (runtimeStats.currentLiquid == CollisionType::Poison || runtimeStats.currentLiquid == CollisionType::Lava) mod = 0.5f;
  else if (runtimeStats.currentLiquid == CollisionType::Water) mod = 0.7f;
  runtimeStats.velocity.x = -baseStats.moveVelocity * (1.0f + buffManager_.getTotalSpeedMultiplier()) * mod;
}

void Player::stopLeftRun() {
  if (runtimeStats.velocity.x < 0.0f)
    runtimeStats.velocity.x = 0.0f;
}

void Player::stopRightRun() {
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

void Player::onHazard() { requestState(dieState); }

void Player::onDie() { 
    Entity::onDie();
    worldStats.position = worldStats.startPosition;
    runtimeStats.velocity = {0.0f, 0.0f};
    runtimeStats.health = baseStats.maxHealth;
    runtimeStats.mana = baseStats.maxMana;
    clearEffects();
    buffManager_.clear(*this);
    requestState(idleState); 
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