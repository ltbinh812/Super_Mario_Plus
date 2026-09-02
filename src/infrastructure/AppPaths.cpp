#include "infrastructure/AppPaths.h"
#include "raylib.h"
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

const std::string& AppPaths::SavesDir() {
    // Tính một lần cho cả phiên chạy. GetApplicationDirectory() trả về con trỏ
    // tới bộ đệm tĩnh bên trong raylib, gọi hàm raylib khác có thể ghi đè lên
    // đó — nên phải chép ra std::string ngay và giữ lại.
    static const std::string dir = [] {
        fs::path exeDir(GetApplicationDirectory());

        // Bỏ dấu phân cách cuối để filename() đọc được đúng tên thư mục.
        if (exeDir.has_filename() == false) {
            exeDir = exeDir.parent_path();
        }

        // NHẢY RA KHỎI THƯ MỤC BUILD.
        //
        // .exe nằm trong build/, nên neo thẳng vào thư mục .exe sẽ đặt bản lưu
        // ở build/saves/ — mà build/ là thư mục dùng một lần: xoá đi để build
        // lại hoặc đổi trình biên dịch là mất sạch tiến độ người chơi. Đó là
        // cái bẫy chờ sẵn, không phải giả thuyết.
        //
        // Nên nếu .exe nằm trong một thư mục build tạm, lùi lên một cấp để lấy
        // gốc dự án. Kết quả: <goc-du-an>/saves/ — đúng thư mục người chơi nhìn
        // thấy, sống sót qua mọi lần dọn build, và vẫn KHÔNG phụ thuộc vào việc
        // bấm chạy từ đâu.
        //
        // Nếu sau này đem .exe đi nơi khác (thư mục không tên build), nó dùng
        // luôn thư mục đó — bản đóng gói vẫn gọn trong một chỗ.
        const std::string folder = exeDir.filename().string();
        if (folder == "build" || folder == "Debug" || folder == "Release" ||
            folder == "bin"   || folder == "out") {
            exeDir = exeDir.parent_path();
        }

        return (exeDir / "saves").string();
    }();
    return dir;
}

std::string AppPaths::SettingsFile() {
    return SavesDir() + "/settings.json";
}

std::string AppPaths::CustomMapDir() {
    return SavesDir() + "/custom_map";
}

void AppPaths::MigrateLegacySaves() {
    std::error_code ec;

    const fs::path legacy = "saves";                 // tương đối -> theo thư mục làm việc
    const fs::path target = fs::path(SavesDir());

    if (!fs::exists(legacy, ec)) return;             // chưa từng chạy kiểu cũ

    // Chạy game ngay tại thư mục chứa .exe thì hai đường dẫn trỏ cùng một chỗ,
    // không có gì để dời. So bằng equivalent() chứ không so chuỗi, vì "saves"
    // và "D:/.../build/saves" là hai chuỗi khác nhau nhưng có thể cùng một thư mục.
    if (fs::exists(target, ec) && fs::equivalent(legacy, target, ec)) return;

    fs::create_directories(target, ec);
    if (ec) {
        std::cerr << "[AppPaths] Khong tao duoc " << target.string()
                  << ": " << ec.message() << "\n";
        return;
    }

    // Duyệt cây thủ công thay vì gọi thẳng fs::copy(recursive).
    //
    // Trên libstdc++ (MinGW), fs::copy vào một thư mục đích ĐÃ TỒN TẠI trả về
    // lỗi "File exists" và bỏ ngang toàn bộ, kể cả khi đã bật skip_existing —
    // mà ở đây thư mục đích vừa được create_directories ở ngay trên. Tự đi từng
    // mục thì kiểm soát được: thư mục thì tạo, file đã có thì bỏ qua, và một
    // file lỗi không làm hỏng cả lượt dời.
    int copied = 0;
    for (auto it = fs::recursive_directory_iterator(legacy, ec);
         !ec && it != fs::recursive_directory_iterator(); ++it) {
        const fs::path rel = fs::relative(it->path(), legacy, ec);
        if (ec) { ec.clear(); continue; }
        const fs::path dst = target / rel;

        std::error_code entryEc;
        if (it->is_directory(entryEc)) {
            fs::create_directories(dst, entryEc);
        } else if (!fs::exists(dst, entryEc)) {
            fs::create_directories(dst.parent_path(), entryEc);
            fs::copy_file(it->path(), dst, entryEc);
            if (!entryEc) ++copied;
        }
        if (entryEc) {
            std::cerr << "[AppPaths] Bo qua \"" << rel.string()
                      << "\": " << entryEc.message() << "\n";
        }
    }

    // Cố ý KHÔNG xoá thư mục cũ: nếu có gì sai sót thì bản gốc vẫn còn đó.
    if (copied > 0) {
        TraceLog(LOG_INFO, "[AppPaths] Da doi %d file du lieu luu cu ve \"%s\"",
                 copied, target.string().c_str());
    }
}
