#include "BossIntroState.h"
#include "Boss.h"
#include "EnemyStates/EnemyRunState.h"
#include <iostream>

void BossIntroState::enter(Mob& mob) {
    // We try to play "intro" animation. If not found, it falls back (or we set to idle).
    mob.setAnimation("intro");
    mob.setVelocity({0, 0});
    isCutsceneFinished = false;
}

void BossIntroState::decideAction(Mob& mob) {
}

void BossIntroState::process(Mob& mob) {
    // Check anti-popping condition: Cutscene is done AND animation is done.
    if (isCutsceneFinished) {
        bool animFinished = false;
        if (mob.hasStandardAnimations() && mob.getCurrentStandardAnim()) {
            if (mob.getCurrentStandardAnim()->isFinished()) {
                animFinished = true;
            } else if (mob.getCurrentStandardAnim()->isLooping()) {
                animFinished = true;
            }
        } else if (mob.getCurrentAnim()) {
            if (mob.getCurrentAnim()->isFinished()) {
                animFinished = true;
            } else if (mob.getCurrentAnim()->isLooping()) {
                // If the intro animation loops, or if it doesn't have an intro and falls back to idle,
                // we should not wait for it to finish because it never will.
                animFinished = true;
            }
        }

        if (animFinished) {
            std::cout << "[BossIntroState] Cutscene and animation finished. Entering Chase State.\n";
            Boss* boss = dynamic_cast<Boss*>(&mob);
            if (boss) {
                mob.changeState(std::make_unique<EnemyRunState>());
            }
        }
    }
}

void BossIntroState::exit(Mob& mob) {
}
