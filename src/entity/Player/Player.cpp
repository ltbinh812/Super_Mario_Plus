#include "Player.h"
#include "AssetManager.h"
#include "ISkill.h"
#include "PlayerCommands.h"
#include "raylib.h"
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
}

void Player::update(float dt) {
  if (runtimeStats.iframeTimer > 0.0f) {
      runtimeStats.iframeTimer -= dt;
  }
  
  if (currentState) {
    currentState->update(dt);
  }
  if (worldStats.animation) {
    worldStats.animation->update(dt);
  }
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
  std::cout << name << '\n';
  auto it = animationList.find(name);
  if (it != animationList.end()) {
    worldStats.animation = &it->second;
    worldStats.animation->resetAnimation();
    worldStats.animation->setLoop(loop);
  }
}

void Player::moveRight() {
  worldStats.isFacingRight = true;
  runtimeStats.velocity.x = baseStats.moveVelocity;
}

void Player::moveLeft() {
  worldStats.isFacingRight = false;
  runtimeStats.velocity.x = -baseStats.moveVelocity;
}

void Player::stopLeftRun() {
  if (runtimeStats.velocity.x < 0.0f)
    runtimeStats.velocity.x = 0.0f;
}

void Player::stopRightRun() {
  if (runtimeStats.velocity.x > 0.0f)
    runtimeStats.velocity.x = 0.0f;
}

void Player::jump() { runtimeStats.velocity.y = baseStats.jumpVelocity; }

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
  const float swimSpeed = baseStats.moveVelocity * 0.7f;
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
  return {worldRect, skill->getAttackPower(), skill->getDefensePower(), this};
}

void Player::takeDamage(int damage, float knockbackDirX) {
  if (currentState == &dieState || currentState == &hurtState || runtimeStats.iframeTimer > 0.0f)
    return;
  runtimeStats.health -= damage;
  runtimeStats.iframeTimer = 1.0f; // 1 second of invincibility
  
  addFloatingText(std::to_string(damage), GREEN, {0, 0}, 0.5f);
  
  // Apply a small knockback upwards and horizontally
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

void Player::onEnterWater() { requestState(swimState); }

void Player::onOverlapLadder() {
  // Auto-enter climb only if player explicitly presses climb key (onClimb),
  // not auto. Leave this hook for audio/visual feedback only.
}

void Player::onHazard() { requestState(dieState); }

void Player::onDie() { requestState(dieState); }

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
  cmd.type = EntityType::Explosion;
  cmd.position = worldStats.position; // Centered on the player
  cmd.isFacingRight = worldStats.isFacingRight;
  cmd.ownerName = baseStats.name;
  cmd.spawner = this;

  commandQueue->push(cmd);
}