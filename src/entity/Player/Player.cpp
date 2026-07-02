#include "Player.h"
#include "AssetManager.h"
#include "EntityCommands.h"
#include "raylib.h"

Player::Player(const CharacterStats &charStats, Vector2 pos, Vector2 boxsize,
               bool isRight)
    : Entity(pos, boxsize), isFacingRight(isRight), isGrounded(true),
      airSpeed(1.0f), stats(charStats), currentState(nullptr) {
  changeState(&idleState);
}


void Player::handleInput() {
}

void Player::process() {
  if (!isMovingLeft && !isMovingRight) {
    stopMove();
  }
  isMovingLeft = false;
  isMovingRight = false;
}

void Player::jump() { currentState->onJump(*this); }

void Player::moveRight() {
  isMovingRight = true;
  currentState->onMoveRight(*this);
}

void Player::moveLeft() {
  isMovingLeft = true;
  currentState->onMoveLeft(*this);
}

void Player::stopMove() { currentState->onStopMove(*this); }

std::string Player::getSkill1Name() {
  if (stats.skills.size() > 0) {
    return stats.skills[0];
  }
  return "";
}

std::string Player::getSkill2Name() {
  if (stats.skills.size() > 1) {
    return stats.skills[1];
  }
  return "";
}

void Player::addSkill(const std::string &name, std::unique_ptr<ISkill> skill) {
  skillManager.addSkill(name, std::move(skill));
}

void Player::useSkill(const std::string &skillName) {
  skillManager.useSkill(skillName, *this);
}

void Player::update(float dt) {
  currentState->update(*this, dt);
  processRequest();
  currentAnimation->update(dt);
  skillManager.update(dt);

  // --- HORIZONTAL PHYSICS ---
  float accX = stats.acceleration;
  float friction =
      1500.0f; // sau này ma sát sẽ được đọc từ file json của thông số từng map

  if (isMovingRight) {
    if (velocity.x < 0) {
      velocity.x += friction * 2.0f * dt; // Phanh mượt mà nhưng rất nhanh
      if (velocity.x > 0) velocity.x = 0; // Tránh vọt lố
    } else if (velocity.x < stats.maxSpeed) {
      velocity.x += accX * dt;
      if (velocity.x > stats.maxSpeed) velocity.x = stats.maxSpeed;
    } else if (velocity.x > stats.maxSpeed) {
      velocity.x -= friction * dt;
      if (velocity.x < stats.maxSpeed) velocity.x = stats.maxSpeed;
    }
    setFaceDirection(true);
  } else if (isMovingLeft) {
    if (velocity.x > 0) {
      velocity.x -= friction * 2.0f * dt; // Phanh mượt mà nhưng rất nhanh
      if (velocity.x < 0) velocity.x = 0; // Tránh vọt lố
    } else if (velocity.x > -stats.maxSpeed) {
      velocity.x -= accX * dt;
      if (velocity.x < -stats.maxSpeed) velocity.x = -stats.maxSpeed;
    } else if (velocity.x < -stats.maxSpeed) {
      velocity.x += friction * dt;
      if (velocity.x > -stats.maxSpeed) velocity.x = -stats.maxSpeed;
    }
    setFaceDirection(false);
  } else {
    // Giảm ma sát
    if (velocity.x > 0) {
      velocity.x -= friction * dt;
      if (velocity.x < 0)
        velocity.x = 0;
    } else if (velocity.x < 0) {
      velocity.x += friction * dt;
      if (velocity.x > 0)
        velocity.x = 0;
    }
  }

  velocity.y += stats.gravityScale * 1000.0f * dt;

  prevPosition = position;
  position.x += dt * velocity.x;
  position.y += dt * velocity.y;

  if (position.y > 500) {
    isGrounded = true;
    position.y = 500;
    velocity.y = 0;
  } else {
    isGrounded = false;
  }
}

void Player::render(float alpha) const {
  Rectangle rec = currentAnimation->getCurrentFrame();

  if (!isFacingRight) {
    rec.x += rec.width;
    rec.width = -rec.width;
  }

  Vector2 renderPos = {prevPosition.x + (position.x - prevPosition.x) * alpha,
                       prevPosition.y + (position.y - prevPosition.y) * alpha};

  DrawTextureRec(currentAnimation->getTexture(), rec, renderPos, WHITE);
}

void Player::setRequest(IEntityState<Player> *state) { requestState = state; }

void Player::processRequest() {
  if (requestState != nullptr) {
    changeState(requestState);
    requestState = nullptr;
  }
}

void Player::changeState(IEntityState<Player> *state) {
  if (state == nullptr) {
    std::cerr << "Bad request - State None" << '\n';
    return;
  }

  currentState = state;
  currentState->onEnter(*this);
}

void Player::setAnimation(Animation *anim) {
  if (anim == nullptr) {
    std::cerr << "Bad request - Anim None" << '\n';
    return;
  }

  currentAnimation = anim;
  currentAnimation->resetAnimation();
}

void Player::setFaceDirection(bool isRight) { isFacingRight = isRight; }

bool Player::checkIsGrounded() { return isGrounded; }

void Player::setVelocityX(float vel) { velocity.x = vel; }

void Player::setVelocityY(float vel) { velocity.y = vel; }

void Player::setIsGrounded(bool grounded) { isGrounded = grounded; }
