#ifndef CHARACTERFACTORY_H
#define CHARACTERFACTORY_H

#include "Character.h"
#include <string>
#include <memory>

class CharacterFactory {
public:
    static CharacterFactory& GetInstance() {
        static CharacterFactory instance;
        return instance;
    }

    std::unique_ptr<Character> CreateCharacter(const std::string& type);

private:
    CharacterFactory() = default;
    ~CharacterFactory() = default;
    CharacterFactory(const CharacterFactory&) = delete;
    CharacterFactory& operator=(const CharacterFactory&) = delete;
};

#endif
