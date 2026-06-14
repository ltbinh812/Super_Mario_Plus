#ifndef ABILITYSTRATEGY_H
#define ABILITYSTRATEGY_H

class Character;

class AbilityStrategy {
public:
    virtual ~AbilityStrategy() = default;

    virtual void Execute(Character* character) = 0;
};

#endif
