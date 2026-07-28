#include "Player.h"
#include "AssetManager.h"
#include "PlayerCommands.h"
#include "raylib.h"
#include <iostream>

Player::Player(CharacterBaseStats &bS, CharacterRuntimeStats &rS,
               CharacterWorldStats &wS,
               std::unordered_map<std::string, Animation> animations)
    : Entity(bS, rS, wS), idleState(*this), runState(*this), jumpState(*this),
      fallState(*this), crouchState(*this), hurtState(*this), dieState(*this),
      skillState(*this), animationList(std::move(animations)) {
  currentState = &idleState;
  currentState->onEnter();
}

void Player::update(float dt) {
  if (currentState) {
    currentState->update(dt);
  }
  if (worldStats.animation) {
    worldStats.animation->update(dt);
  }
}

void Player::render(float alpha) {
  if (!worldStats.animation)
    return;

  Rectangle source = worldStats.animation->getCurrentFrame();
  if (!worldStats.isFacingRight) {
    source.width = -source.width; // Flip horizontally
  }

  // Dùng chung 1 tỷ lệ 0.45f cho mọi nhân vật.
  // Lý do: Asset của Goku to gấp đôi Luffy (128x128 so với 64x64) nhưng hình vẽ 
  // nhân vật bên trong lại chiếm tỷ lệ khác nhau. Mức 0.90f sẽ giúp body của cả 2
  // đều đạt mốc ~56px chiều cao (gấp đôi mốc cũ 28px).
  float scale = 0.90f;

  float absW = (source.width < 0 ? -source.width : source.width) * scale;
  float absH = source.height * scale;
  
  // Neo hình ảnh vào giữa và sát dưới chân (bottom-center)
  Rectangle dest = {worldStats.position.x - (absW / 2.0f), worldStats.position.y - absH, absW, absH};

  DrawTexturePro(worldStats.animation->getTexture(), source, dest, {0, 0}, 0.0f,
                 WHITE);
                 
  // Vẽ khung Hitbox màu đỏ để debug xem có khớp với Animation không
  DrawRectangleLinesEx(getHitbox(), 1.0f, RED);
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

void Player::useSkill(const std::string &skillname) {
  auto it = skillList.find(skillname);
  if (it != skillList.end() && it->second) {
    ISkill *skill = it->second.get();
    if (runtimeStats.mana < skill->getManaCost())
      return;
    skillState.setSkill(skill);
    changeState(skillState);
  }
}

void Player::addSkill(const std::string &name, std::unique_ptr<ISkill> skill) {
  skillList[name] = std::move(skill);
}

void Player::onMoveRight() {
  if (currentState)
    currentState->onMoveRight();
}

void Player::onMoveLeft() {
  if (currentState)
    currentState->onMoveLeft();
}

void Player::onJump() {
  if (currentState)
    currentState->onJump();
}

void Player::onStopLeft() {
  if (currentState)
    currentState->onStopLeft();
}

void Player::onStopRight() {
  if (currentState)
    currentState->onStopRight();
}

void Player::onCrouch() {
  if (currentState)
    currentState->onCrouch();
}

void Player::onAttack() {
  if (currentState)
    currentState->onAttack();
}

void Player::playAnimation(const std::string &name) {
  std::cout << name << '\n';
  auto it = animationList.find(name);
  if (it != animationList.end()) {
    worldStats.animation = &it->second;
    worldStats.animation->resetAnimation();
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
  runtimeStats.hitbox.y /= 2;
  runtimeStats.velocity = {0.0f, 0.0f};
}

void Player::dash(float dashSpeed) {
  if (worldStats.isFacingRight) {
    runtimeStats.velocity.x = dashSpeed;
  } else {
    runtimeStats.velocity.x = -dashSpeed;
  }
}

void Player::idle() { runtimeStats.velocity = {0, 0}; }

void Player::reduceMana(float cost) {
  runtimeStats.mana -= static_cast<int>(cost);
  if (runtimeStats.mana < 0)
    runtimeStats.mana = 0;
}

void Player::increaseMana(float cost) {
  runtimeStats.mana += static_cast<int>(cost);
  if (runtimeStats.mana > baseStats.maxMana)
    runtimeStats.mana = baseStats.maxMana;
}

void Player::onLand(float floorY) {
  // Khi Player tiếp đất, isGrounded đã được đặt thành true bởi Entity::updatePhysicsWithMap.
}

void Player::onHitCeiling(float ceilY) {
  // Có thể kích hoạt âm thanh cụng gạch hoặc phá gạch trong tương lai
}

void Player::updateStateFromPhysics() {
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