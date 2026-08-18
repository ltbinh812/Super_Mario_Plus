#pragma once
#include "IItemUseStrategy.h"
#include <memory>
#include <string>

class ItemUsageFactory {
public:
    static std::unique_ptr<IItemUseStrategy> create(const std::string& itemIdentifier);
};
