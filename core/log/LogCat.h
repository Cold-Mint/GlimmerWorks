//
// Created by Cold-Mint on 2025/10/9.
//
#ifndef LOGCAT_H
#define LOGCAT_H
#include <chrono>
#include <iostream>
#include <thread>
#include <iomanip>
#ifdef __ANDROID__
#include <android/log.h>
#endif
#if !defined(__ANDROID__) && defined(__linux__) && !defined(NDEBUG)
#include <spawn.h>
#endif
#define COLOR_RESET   "\033[0m"
#define COLOR_INFO    "\033[32m"  // Green 绿色
#define COLOR_DEBUG   "\033[36m"  // Cyan 青色
#define COLOR_WARN    "\033[33m"  // Yellow 黄色
#define COLOR_ERROR   "\033[31m"  // Red 红色

namespace glimmer {
    class LogCat {
        static std::string CurrentTime();

        static std::string ThreadTag();

    public:
        template<typename... Args>
        static void i(Args &&... args) {
#if  !defined(NDEBUG)
#ifdef __ANDROID__
            std::ostringstream oss;
            (oss << ... << args);
            __android_log_print(ANDROID_LOG_INFO, "GlimmerWorks", "%s", oss.str().c_str());
#else
            std::cout << COLOR_INFO << CurrentTime() << " | INFO |" << ThreadTag();
            (std::cout << ... << args);
            std::cout << COLOR_RESET << std::endl;
#endif
#endif
        }

        template<typename... Args>
        static void d(Args &&... args) {
#if  !defined(NDEBUG)
#ifdef __ANDROID__
            std::ostringstream oss;
            (oss << ... << args);
            __android_log_print(ANDROID_LOG_DEBUG, "GlimmerWorks", "%s", oss.str().c_str());
#else
            std::cout << COLOR_DEBUG << CurrentTime() << " | DEBUG |" << ThreadTag();
            (std::cout << ... << args);
            std::cout << COLOR_RESET << std::endl;
#endif
#endif
        }

        template<typename... Args>
        static void w(Args &&... args) {
#ifdef __ANDROID__
            std::ostringstream oss;
            (oss << ... << args);
            __android_log_print(ANDROID_LOG_WARN, "GlimmerWorks", "%s", oss.str().c_str());
#else
            std::cout << COLOR_WARN << CurrentTime() << " | WARN |" << ThreadTag();
            (std::cout << ... << args);
            std::cout << COLOR_RESET << std::endl;
#endif
        }


#if !defined(__ANDROID__) && defined(__linux__) && !defined(NDEBUG)
        static void NotifySend(const std::string &text);
#endif

        template<typename... Args>
        static void e(Args &&... args) {
#if !defined(__ANDROID__) && defined(__linux__) && !defined(NDEBUG)
            std::ostringstream oss;
            (oss << ... << args);
            NotifySend(oss.str());
#endif
#ifdef __ANDROID__
            std::ostringstream oss;
            (oss << ... << args);
            __android_log_print(ANDROID_LOG_ERROR, "GlimmerWorks", "%s", oss.str().c_str());
#else
            std::cout << COLOR_ERROR << CurrentTime() << " | ERROR |" << ThreadTag();
            (std::cout << ... << args);
            std::cout << COLOR_RESET << std::endl;
#endif
        }
    };
}

#endif //LOGCAT_H
