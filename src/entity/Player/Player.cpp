#include "Player.h"
#include "AssetManager.h"
#include "PlayerCommands.h"
#include "raylib.h"
#include <iostream>

Player::Player(CharacterBaseStats &bS, CharacterRuntimeStats &rS, CharacterWorldStats &wS, std::unordered_map<std::string, Animation> animations) 
: Entity(bS, rS, wS), 
  idleState(*this),
  runState(*this),
  jumpState(*this),
  fallState(*this),
  crouchState(*this),
  hurtState(*this),
  dieState(*this),
  skillState(*this),
  animationList(std::move(animations))
{
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
    if (!worldStats.animation) return;

    Rectangle source = worldStats.animation->getCurrentFrame();
    if (!worldStats.isFacingRight) {
        source.width = -source.width; // Flip horizontally
    }

    // Assuming the position is the top-left of the character
    Vector2 pos = worldStats.position;
    DrawTextureRec(worldStats.animation->getTexture(), source, pos, WHITE);
}

void Player::changeState(PlayerState &requestState) {
    if (currentState == &requestState) return;
    if (currentState) {
        currentState->onExit();
    }
    currentState = &requestState;
    currentState->onEnter();
}

void Player::useSkill(const std::string& skillname) {
    auto it = skillList.find(skillname);
    if (it != skillList.end() && it->second) {
        skillState.setSkill(it->second.get());
        changeState(skillState);
    }
}

void Player::addSkill(const std::string& name, std::unique_ptr<ISkill> skill) {
    skillList[name] = std::move(skill);
}

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
    if (currentState) currentState->onCrouch();
}

void Player::onAttack() {
    if (currentState) currentState->onAttack();
}

void Player::playAnimation(const std::string& name) {
    std::cout << name << '\n';
    auto it = animationList.find(name);
    if (it != animationList.end()) {
        worldStats.animation = &it->second;
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

void Player::jump() {
    runtimeStats.velocity.y = baseStats.jumpVelocity;
}

void Player::crouch() {
    runtimeStats.hitbox.y /= 2;
    runtimeStats.velocity = {0.0f, 0.0f};
    
}