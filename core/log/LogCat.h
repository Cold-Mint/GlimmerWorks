//
// Created by Cold-Mint on 2025/10/9.
//
#ifndef LOGCAT_H
#define LOGCAT_H
#include <iostream>

#define COLOR_RESET   "\033[0m"
#define COLOR_INFO    "\033[32m"  // Green 绿色
#define COLOR_DEBUG   "\033[36m"  // Cyan 青色
#define COLOR_WARN    "\033[33m"  // Yellow 黄色
#define COLOR_ERROR   "\033[31m"  // Red 红色

namespace Glimmer {
    class LogCat {
    public:
        template<typename... Args>
        static void i(Args &&... args) {
#if  !defined(NDEBUG)
            std::cout << COLOR_INFO << "[INFO] ";
            (std::cout << ... << args);
            std::cout << COLOR_RESET << std::endl;
#endif
        }

        template<typename... Args>
        static void d(Args &&... args) {
#if  !defined(NDEBUG)
            std::cout << COLOR_DEBUG << "[DEBUG] ";
            (std::cout << ... << args);
            std::cout << COLOR_RESET << std::endl;
#endif
        }

        template<typename... Args>
        static void w(Args &&... args) {
            std::cout << COLOR_WARN << "[WARN] ";
            (std::cout << ... << args);
            std::cout << COLOR_RESET << std::endl;
        }

        template<typename... Args>
        static void e(Args &&... args) {
            std::cout << COLOR_ERROR << "[ERROR] ";
            (std::cout << ... << args);
            std::cout << COLOR_RESET << std::endl;
        }
    };
}

#endif //LOGCAT_H
