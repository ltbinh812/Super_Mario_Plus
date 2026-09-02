#pragma once

#include <iostream>
#include <streambuf>
#include <raylib.h>

// =============================================================================
// Công tắc log ra console.
//
// Dự án có ~92 lệnh std::cout và ~81 std::cerr rải khắp nơi, cộng với log INFO
// của raylib (một dòng cho MỖI ảnh được nạp). Một lần chạy in ra khoảng 670
// dòng, che lấp hoàn toàn những dòng thật sự cần đọc.
//
// Cách tắt ở đây KHÔNG xoá lệnh nào: chỉ đổi bộ đệm của std::cout/std::cerr
// sang một streambuf rỗng. Giữ nguyên chỗ gọi thì lúc cần soi lỗi chỉ việc bật
// kVerbose lên, không phải đi chép lại hàng trăm dòng log đã xoá.
//
// TraceLog của raylib thì hạ xuống mức WARNING chứ không tắt hẳn: phần ồn ào
// là các dòng INFO, còn WARNING/ERROR mới là chỗ báo thiếu file, hỏng texture,
// hỏng âm thanh — mất chúng là mất luôn manh mối khi asset có vấn đề.
// =============================================================================
namespace DebugLog {

// true = in đầy đủ như trước (dùng khi đang truy lỗi).
inline constexpr bool kVerbose = false;

// Bộ đệm nuốt hết mọi ký tự. overflow() luôn báo thành công nên stream không
// bị dựng cờ lỗi — khác với cách gán rdbuf(nullptr), vốn làm stream vào trạng
// thái bad và có thể khiến code kiểm tra std::cout đi nhánh sai.
class NullBuffer : public std::streambuf {
protected:
    int overflow(int c) override { return c; }
};

// Gọi ĐÚNG MỘT LẦN, càng sớm càng tốt trong main().
inline void Apply() {
    if (kVerbose) {
        SetTraceLogLevel(LOG_ALL);
        return;
    }
    SetTraceLogLevel(LOG_WARNING);

    // static: bộ đệm phải sống lâu hơn mọi lần ghi vào stream, kể cả trong
    // các destructor chạy lúc thoát chương trình.
    static NullBuffer nullBuf;
    std::cout.rdbuf(&nullBuf);
    std::cerr.rdbuf(&nullBuf);
    std::clog.rdbuf(&nullBuf);
}

}  // namespace DebugLog
