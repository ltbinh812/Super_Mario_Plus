#include "ItemFactory.h"
#include "Spring.h"
#include "Coin.h"
#include "Key.h"
#include "Door.h"
#include "Flag.h"
#include "LuckyBlock.h"
#include "ChestNormal.h"
#include "ChestBoss.h"
#include "Buff.h"
#include "Boom.h"
#include "nlohmann/json.hpp"
#include <iostream>

using json = nlohmann::json;

std::unique_ptr<BaseItem> ItemFactory::create(
    const std::string& identifier,
    Vector2 worldPos,
    const json& fieldInstances)
{
    if (identifier == "Spring_down")  return std::make_unique<Spring>(worldPos, SpringDir::Down);
    if (identifier == "Spring_up")    return std::make_unique<Spring>(worldPos, SpringDir::Up);
    if (identifier == "Spring_left")  return std::make_unique<Spring>(worldPos, SpringDir::Left);
    if (identifier == "Spring_right") return std::make_unique<Spring>(worldPos, SpringDir::Right);
    if (identifier == "Coin")         return std::make_unique<Coin>(worldPos);
    if (identifier == "Key")          return std::make_unique<Key>(worldPos);
    if (identifier == "Door")         return std::make_unique<Door>(worldPos);
    if (identifier == "Flag")         return std::make_unique<Flag>(worldPos);
    if (identifier == "Luckyblock" || identifier == "LuckyBlock")
                                      return std::make_unique<LuckyBlock>(worldPos);
    if (identifier == "Chest_normal") return std::make_unique<ChestNormal>(worldPos);
    if (identifier == "Chest_boss")   return std::make_unique<ChestBoss>(worldPos);
    if (identifier == "Buff" || identifier == "Item") {
        std::string itemType = "";
        if (fieldInstances.is_array()) {
            for (const auto& field : fieldInstances) {
                if (field.value("__identifier", "") == "item_type") {
                    itemType = field.value("__value", "");
                    break;
                }
            }
        }
        return std::make_unique<Buff>(worldPos, 2.0f, itemType);
    }
    if (identifier == "Boom")         return std::make_unique<Boom>(worldPos);

    std::cout << "[ItemFactory] Unknown identifier: " << identifier << "\n";
    return nullptr;
}

std::unique_ptr<BaseItem> ItemFactory::createDynamic(
    const std::string& identifier,
    Vector2 worldPos)
{
    return createDynamic(identifier, worldPos, {0.0f, 0.0f});
}

std::unique_ptr<BaseItem> ItemFactory::createDynamic(
    const std::string& identifier,
    Vector2 worldPos,
    Vector2 initialVelocity)
{
    std::unique_ptr<BaseItem> item = nullptr;

    if (identifier == "Coin")         item = std::make_unique<Coin>(worldPos);
    else if (identifier == "Key")     item = std::make_unique<Key>(worldPos);
    else if (identifier == "Boom")    item = std::make_unique<Boom>(worldPos);
    else if (identifier == "Buff")    item = std::make_unique<Buff>(worldPos, 2.0f, "");
    else if (identifier == "ThrownBoom") {
        // Pre-activated boom thrown by player — uses Boom(pos, velocity) constructor
        item = std::make_unique<Boom>(worldPos, initialVelocity);
    }

    if (item) {
        if (identifier != "ThrownBoom") {
            item->setPickupDelay(0.5f); // normal spawned items need delay
        }
        return item;
    }

    std::cout << "[ItemFactory] Unknown dynamic identifier: " << identifier << "\n";
    return nullptr;
}
