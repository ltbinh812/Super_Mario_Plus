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
    public:
    Player(CharacterBaseStats &bS, CharacterRuntimeStats &rS, CharacterWorldStats &wS, std::unordered_map<std::string, Animation> animations);
    void update(float dt) override;
    void render(float alpha) override;
    void changeState(PlayerState &requestState);
    
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
    void moveRight();
    void moveLeft();
    void stopLeftRun();
    void stopRightRun();
    void jump();
    void crouch();



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
    
    PlayerState *currentState;
    std::unordered_map<std::string, Animation> animationList;
    std::unordered_map<std::string, std::unique_ptr<ISkill>> skillList;

     
};