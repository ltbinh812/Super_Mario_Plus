#pragma once
#include "Entity.h"
#include "CharacterStats.h"
#include <string>
#include <map>
#include <unordered_map>
#include <memory>
#include <PlayerStates.h>

class ISkill;


class Player : public Entity {
    friend class PlayerState;

    public:
    Player(CharacterBaseStats &bS, CharacterRuntimeStats &rS, CharacterWorldStats &wS, std::unordered_map<std::string, Animation> animations);
    void update(float dt) override;
    void render(float alpha) override;
    void requestState(PlayerState &state);
    
    void useSkill(const std::string& skillname);
    void addSkill(const std::string& name, std::unique_ptr<ISkill> skill);

    // Actions
    void onMoveRight();
    void onMoveLeft();
    void onJump();
    void onStopLeft();
    void onStopRight();
    void onCrouch();
    void onAttack();

    
    //helper functions
    void idle();
    void moveRight();
    void moveLeft();
    void stopLeftRun();
    void stopRightRun();
    void jump();
    void crouch();
    void dash(float dashSpeed);
    void reduceMana(float cost);
    void increaseMana(float cost);

    // Physics helpers
    void applyGravity(float dt);
    void updatePosition(float dt);
    void checkGroundCollision(float groundY);
    void updateStateFromPhysics();


    // list of States
    PlayerIdleState idleState;
    PlayerRunState runState;
    PlayerJumpState jumpState;
    PlayerFallState fallState;
    PlayerCrouchState crouchState;
    PlayerHurtState hurtState;
    PlayerDieState dieState;
    PlayerSkillState skillState;
    void playAnimation(const std::string& name);


    private:
    void changeState(PlayerState &state);
    
    PlayerState *currentState;
    std::unordered_map<std::string, Animation> animationList;
    std::unordered_map<std::string, std::unique_ptr<ISkill>> skillList;

     
};