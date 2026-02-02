//
// Created by Cold-Mint on 2025/10/9.
//
#include "LogCat.h"

std::string glimmer::LogCat::CurrentTime() {
    using namespace std::chrono;
    const auto now = system_clock::now();
    std::time_t t = system_clock::to_time_t(now);
    std::tm localTime{};
    localtime_r(&t, &localTime);
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

#if !defined(__ANDROID__) && defined(__linux__) && !defined(NDEBUG)
void glimmer::LogCat::NotifySend(const std::string &text) {
    pid_t pid;

    const char *argv[] = {
        "notify-send",
        "GlimmerWorks",
        text.c_str(),
        nullptr
    };

    if (posix_spawnp(
            &pid,
            "notify-send",
            nullptr,
            nullptr,
            const_cast<char * const*>(argv),
            environ
        ) != 0) {
        // spawn failed
        // spawn 失败
        return;
    }
}
#endif