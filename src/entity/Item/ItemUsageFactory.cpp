#include "ItemUsageFactory.h"
#include "ThrowBombStrategy.h"
#include "ConsumeBuffStrategy.h"

std::unique_ptr<IItemUseStrategy> ItemUsageFactory::create(const std::string& itemIdentifier) {
    if (itemIdentifier == "Boom") {
        return std::make_unique<ThrowBombStrategy>();
    }
    
    // Check if it's a known buff
    if (itemIdentifier == "Speed" || itemIdentifier == "Strength" || 
        itemIdentifier == "Shield" || itemIdentifier == "Jump" || 
        itemIdentifier == "Invisibility" || itemIdentifier == "GoldMagnet" || 
        itemIdentifier == "TimeStop") {
        return std::make_unique<ConsumeBuffStrategy>(itemIdentifier);
    }
    
    return nullptr;
}
