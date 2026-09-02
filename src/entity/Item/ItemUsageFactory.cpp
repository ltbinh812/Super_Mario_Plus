#include "ItemUsageFactory.h"
#include "ThrowBombStrategy.h"
#include "ThrowPoisonStrategy.h"
#include "ConsumeBuffStrategy.h"

const std::vector<std::string>& ItemUsageFactory::allUsableItems() {
    // 8 buff + 2 vật phẩm ném. Thứ tự không quan trọng; đây là bể để bốc ngẫu nhiên.
    static const std::vector<std::string> kItems = {
        "Speed", "Strength", "Shield", "Jump",
        "Invisibility", "GoldMagnet", "TimeStop", "Heal",
        "Poison", "Boom"
    };
    return kItems;
}

std::unique_ptr<IItemUseStrategy> ItemUsageFactory::create(const std::string& itemIdentifier) {
    if (itemIdentifier == "Boom") {
        return std::make_unique<ThrowBombStrategy>();
    }
    if (itemIdentifier == "Poison") {
        return std::make_unique<ThrowPoisonStrategy>();
    }
    
    // Check if it's a known buff
    if (itemIdentifier == "Speed" || itemIdentifier == "Strength" || 
        itemIdentifier == "Shield" || itemIdentifier == "Jump" || 
        itemIdentifier == "Invisibility" || itemIdentifier == "GoldMagnet" || 
        itemIdentifier == "TimeStop" || itemIdentifier == "Heal") {
        return std::make_unique<ConsumeBuffStrategy>(itemIdentifier);
    }
    
    return nullptr;
}
