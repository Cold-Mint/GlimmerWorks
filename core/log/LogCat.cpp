//
// Created by Cold-Mint on 2025/10/9.
//
#include "LogCat.h"

std::string glimmer::LogCat::CurrentTime() {
    using namespace std::chrono;
    const auto now = system_clock::now();
    std::time_t t = system_clock::to_time_t(now);
    std::tm localTime{};

    // 关键：跨平台兼容 Linux/Windows
#ifdef _WIN32  // Windows 平台（mingw-w64 会定义这个宏）
    localtime_s(&localTime, &t);  // Windows 函数，参数顺序和 Linux 相反
#else  // Linux/Mac 平台
    localtime_r(&t, &localTime);  // Linux 原生函数
#endif

    std::array<char, 32> buffer{};
    (void) std::strftime(buffer.data(), buffer.size(), "%Y-%m-%d %H:%M:%S", &localTime);
    const auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;
    std::ostringstream oss;
    oss << buffer.data() << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return oss.str();
}

std::string glimmer::LogCat::ThreadTag() {
    std::ostringstream oss;
    oss << std::this_thread::get_id() << "| ";
    return oss.str();
}