#pragma once
#include "IItemUseStrategy.h"
#include "Player.h"
#include "SpeedBuff.h"
#include "StrengthBuff.h"
#include "ShieldBuff.h"
#include "JumpBuff.h"
#include "InvisiBuff.h"
#include "GoldMagnetBuff.h"
#include "TimeStopBuff.h"
#include "HealBuff.h"
#include <string>
#include <iostream>

class ConsumeBuffStrategy : public IItemUseStrategy {
public:
    ConsumeBuffStrategy(const std::string& buffType) : buffType_(buffType) {}

    void use(Player& player) override {
        std::unique_ptr<IBuffEffect> buff = nullptr;
        if (buffType_ == "Speed") buff = std::make_unique<SpeedBuff>();
        else if (buffType_ == "Strength") buff = std::make_unique<StrengthBuff>();
        else if (buffType_ == "Shield") buff = std::make_unique<ShieldBuff>();
        else if (buffType_ == "Jump") buff = std::make_unique<JumpBuff>();
        else if (buffType_ == "Invisibility") buff = std::make_unique<InvisiBuff>();
        else if (buffType_ == "GoldMagnet") buff = std::make_unique<GoldMagnetBuff>();
        else if (buffType_ == "TimeStop") buff = std::make_unique<TimeStopBuff>();
        else if (buffType_ == "Heal") buff = std::make_unique<HealBuff>();

        if (buff) {
            player.getBuffManager().addBuff(std::move(buff), player);
            std::cout << "[ConsumeBuffStrategy] Consumed " << buffType_ << " buff!\n";
        }
    }
private:
    std::string buffType_;
};
