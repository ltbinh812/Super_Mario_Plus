#pragma once
#include "IItemUseStrategy.h"
#include <memory>
#include <string>
#include <vector>

class ItemUsageFactory {
public:
    static std::unique_ptr<IItemUseStrategy> create(const std::string& itemIdentifier);

    // Mọi định danh vật phẩm mà người chơi thật sự CẦM và DÙNG được.
    //
    // Trước đây danh sách này bị chép tay ở ba nơi — create() ở dưới, bảng quay
    // của Random buff, và bảng rơi đồ của rương — và cả ba đều lệch nhau:
    // Random buff bỏ sót Strength, Invisibility, TimeStop, còn nhánh trả về
    // "Boom" của nó là code chết vì rand()%6 không bao giờ chạm tới.
    //
    // Gom về một chỗ thì thêm một vật phẩm mới chỉ phải sửa đúng đây.
    static const std::vector<std::string>& allUsableItems();
};
