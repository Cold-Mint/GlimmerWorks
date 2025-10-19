//
// Created by Cold-Mint on 2025/10/9.
//
#ifndef LOGCAT_H
#define LOGCAT_H
#include <chrono>
#include <iostream>
#include <thread>

#define COLOR_RESET   "\033[0m"
#define COLOR_INFO    "\033[32m"  // Green 绿色
#define COLOR_DEBUG   "\033[36m"  // Cyan 青色
#define COLOR_WARN    "\033[33m"  // Yellow 黄色
#define COLOR_ERROR   "\033[31m"  // Red 红色

namespace Glimmer {
    class LogCat {
        static std::string currentTime() {
            using namespace std::chrono;
            const auto now = system_clock::now();
            std::time_t t = system_clock::to_time_t(now);
            std::tm localTime{};
            localtime_r(&t, &localTime);
            char buffer[32];
            std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &localTime);
            const auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;
            std::ostringstream oss;
            oss << buffer << '.' << std::setfill('0') << std::setw(3) << ms.count();
            return oss.str();
        }

        static std::string threadTag() {
            std::ostringstream oss;
            oss << std::this_thread::get_id() << "| ";
            return oss.str();
        }

    public:
        template<typename... Args>
        static void i(Args &&... args) {
#if  !defined(NDEBUG)
            std::cout << COLOR_INFO << currentTime() << " | INFO |" << threadTag();
            (std::cout << ... << args);
            std::cout << COLOR_RESET << std::endl;
#endif
        }

        template<typename... Args>
        static void d(Args &&... args) {
#if  !defined(NDEBUG)
            std::cout << COLOR_DEBUG << currentTime() << " | DEBUG |" << threadTag();
            (std::cout << ... << args);
            std::cout << COLOR_RESET << std::endl;
#endif
        }

        template<typename... Args>
        static void w(Args &&... args) {
            std::cout << COLOR_WARN << currentTime() << " | WARN |" << threadTag();
            (std::cout << ... << args);
            std::cout << COLOR_RESET << std::endl;
        }

        template<typename... Args>
        static void e(Args &&... args) {
            std::cout << COLOR_ERROR << currentTime() << " | ERROR |" << threadTag();
            (std::cout << ... << args);
            std::cout << COLOR_RESET << std::endl;
        }
    };
}

#endif //LOGCAT_H
