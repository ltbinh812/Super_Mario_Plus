#pragma once
class Player;

class IItemUseStrategy {
public:
    virtual ~IItemUseStrategy() = default;
    virtual void use(Player& player) = 0;
};
