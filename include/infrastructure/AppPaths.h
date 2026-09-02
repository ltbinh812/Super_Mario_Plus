#pragma once
#include <string>

// =============================================================================
// AppPaths — MỘT nơi duy nhất quyết định game ghi dữ liệu người chơi ở đâu.
//
// VẤN ĐỀ NÓ GIẢI QUYẾT
// --------------------
// Trước đây ba hệ thống tự chế đường dẫn riêng, và cả ba đều là chuỗi TƯƠNG ĐỐI:
//
//     SaveManager         -> "saves"                      (bản lưu màn chơi)
//     SettingsManager     -> "saves/settings.json"        (phím + âm lượng)
//     CustomMapSerializer -> "saves/custom_map/..."       (map tự tạo)
//
// Đường dẫn tương đối được tính theo THƯ MỤC LÀM VIỆC của tiến trình, mà thư
// mục đó phụ thuộc vào cách người chơi bấm chạy:
//
//     mở từ VS Code / thư mục gốc  ->  <goc-du-an>/saves/...
//     bấm thẳng build/game.exe     ->  <goc-du-an>/build/saves/...
//
// Nghĩa là cùng một cái máy, cùng một bản game, nhưng hai kiểu mở lại đọc/ghi
// hai thư mục khác nhau. Đổi phím ở lần chạy này, lần sau mở kiểu khác là như
// chưa từng đổi; bản lưu màn chơi cũng "biến mất" y hệt.
//
// CÁCH CHỮA
// ---------
// Neo mọi thứ vào THƯ MỤC CHỨA FILE .EXE (raylib GetApplicationDirectory()).
// Đường dẫn này cố định bất kể bấm chạy từ đâu, nên chỉ còn đúng một thư mục
// saves/. Nó cũng là chỗ đúng khi đem nộp bài: CMake đã chép assets/ sang cạnh
// .exe, giờ saves/ cũng nằm cạnh .exe — cả game gói gọn trong một thư mục.
//
// Toàn bộ dữ liệu người chơi nằm dưới đúng một gốc:
//
//     <thu-muc-exe>/saves/
//         settings.json           <- phím, âm lượng
//         world01/version1.json   <- bản lưu màn chơi
//         custom_map/custom_map_0.json
// =============================================================================
class AppPaths {
public:
    // "<thu-muc-exe>/saves" — gốc chung của mọi dữ liệu người chơi.
    // Tính một lần rồi nhớ lại, vì GetApplicationDirectory() dùng bộ đệm tĩnh
    // của raylib và có thể bị lệnh khác ghi đè.
    static const std::string& SavesDir();

    static std::string SettingsFile();   // <saves>/settings.json
    static std::string CustomMapDir();   // <saves>/custom_map

    // Dời dữ liệu cũ về gốc mới, chạy đúng một lần lúc khởi động.
    //
    // Người chơi đã lỡ chạy game theo kiểu cũ thì đang có sẵn một thư mục
    // "saves" tính theo thư mục làm việc, bên trong là bản lưu và map tự tạo
    // thật. Nếu cứ thế đổi sang gốc mới thì những thứ đó thành vô hình.
    //
    // Hàm này chép những gì CHƯA có ở gốc mới sang (không ghi đè, không xoá bản
    // gốc), nên chạy lại nhiều lần vẫn an toàn và không mất gì.
    static void MigrateLegacySaves();
};
