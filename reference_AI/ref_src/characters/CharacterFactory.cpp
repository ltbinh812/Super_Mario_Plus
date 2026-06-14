#include "CharacterFactory.h"
#include "Player.h"
#include "Mario.h"
#include "Luigi.h"
#include "Peach.h"
#include "Toad.h"
#include "Wario.h"
#include "Goomba.h"
#include "FireballAbility.h"
#include "HighJumpAbility.h"
#include "FloatAbility.h"
#include "DashAbility.h"
#include "GroundPoundAbility.h"

std::unique_ptr<Character> CharacterFactory::CreateCharacter(const std::string& type) {
    if (type == "Mario" || type == "Player") {
        auto c = std::make_unique<Mario>();
        c->SetAbility1(std::make_unique<FireballAbility>());
        c->SetAbility2(std::make_unique<DashAbility>());
        return c;
    }
    if (type == "Luigi") {
        auto c = std::make_unique<Luigi>();
        c->SetAbility1(std::make_unique<HighJumpAbility>());
        c->SetAbility2(std::make_unique<DashAbility>());
        return c;
    }
    if (type == "Peach") {
        auto c = std::make_unique<Peach>();
        c->SetAbility1(std::make_unique<FloatAbility>());
        c->SetAbility2(std::make_unique<DashAbility>());
        return c;
    }
    if (type == "Toad") {
        auto c = std::make_unique<Toad>();
        c->SetAbility1(std::make_unique<DashAbility>());
        c->SetAbility2(std::make_unique<HighJumpAbility>());
        return c;
    }
    if (type == "Wario") {
        auto c = std::make_unique<Wario>();
        c->SetAbility1(std::make_unique<GroundPoundAbility>());
        c->SetAbility2(std::make_unique<DashAbility>());
        return c;
    }
    if (type == "Goomba") {
        return std::make_unique<Goomba>();
    }
    return nullptr;
}
