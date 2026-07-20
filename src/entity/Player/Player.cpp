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

    // Position is the bottom-left (feet) of the character
    // Offset upward by sprite height for drawing
    Vector2 pos = {
        worldStats.position.x,
        worldStats.position.y - source.height
    };
    DrawTextureRec(worldStats.animation->getTexture(), source, pos, WHITE);
}

void Player::changeState(PlayerState &state) {
    if (currentState == &state) return;
    if (currentState) {
        currentState->onExit();
    }
    currentState = &state;
    currentState->onEnter();
}

void Player::requestState(PlayerState &state) {
    if (currentState && !currentState->canExit()) return;
    changeState(state);
}

void Player::useSkill(const std::string& skillname) {
    auto it = skillList.find(skillname);
    if (it != skillList.end() && it->second) {
        ISkill* skill = it->second.get();
        if (runtimeStats.mana < skill->getManaCost()) return;
        skillState.setSkill(skill);
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

void Player::jump() {
    runtimeStats.velocity.y = baseStats.jumpVelocity;
}

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

void Player::idle() {
    runtimeStats.velocity = {0 , 0};
}

void Player::reduceMana(float cost) {
    runtimeStats.mana -= static_cast<int>(cost);
    if (runtimeStats.mana < 0) runtimeStats.mana = 0;
}

void Player::increaseMana(float cost) {
    runtimeStats.mana += static_cast<int>(cost);
    if (runtimeStats.mana > baseStats.maxMana) runtimeStats.mana = baseStats.maxMana;
}

void Player::applyGravity(float dt) {
    runtimeStats.velocity.y += baseStats.gravityScale * 9.8f * dt;
}

void Player::updatePosition(float dt) {
    worldStats.position.x += runtimeStats.velocity.x * dt;
    worldStats.position.y += runtimeStats.velocity.y * dt;
}

void Player::checkGroundCollision(float groundY) {
    if (worldStats.position.y >= groundY) {
        worldStats.position.y = groundY;
        runtimeStats.velocity.y = 0.0f;
        runtimeStats.isGrounded = true;
    } else {
        runtimeStats.isGrounded = false;
    }
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