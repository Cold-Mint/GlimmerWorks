/*
 * Copyright (C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * 版权(C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * 本程序是自由软件：你可以遵照自由软件基金会出版的GNU Affero通用公共许可证条款来重新分发和修改它
 * 该许可证的第3版，或者（由你选择）任何后续版本。
 *
 * 本程序的发布目的是希望它能有用，但没有任何担保；甚至没有适销性或特定用途适用性的默示担保。
 * 有关详细细节，请参阅GNU Affero通用公共许可证。
 *
 * 你应该已经收到一份GNU Affero通用公共许可证的副本。如果没有，请查阅<https://www.gnu.org/licenses/>。
 */
#pragma once
#include <assert.h>
#include <atomic>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <memory>
#include <source_location>
#include <sstream>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <fmt/format.h>

#include "ErrorCode.h"
#include "LogLabel.h"
#ifdef __ANDROID__
#include <android/log.h>
#endif
#if defined(_WIN32)
#include <windows.h>
#else
#include <pthread.h>
#endif
constexpr const char *COLOR_RESET = "\o{33}[0m";
constexpr const char *COLOR_INFO = "\o{33}[32m";
constexpr const char *COLOR_DEBUG = "\o{33}[36m";
constexpr const char *COLOR_WARN = "\o{33}[33m";
constexpr const char *COLOR_ERROR = "\o{33}[31m";

namespace glimmer {
    inline thread_local std::string logThreadName_;

    class LogCat {
        using LogTable = std::unordered_map<std::string, std::string>;

        static inline std::atomic<std::shared_ptr<const LogTable> > localizer_ = nullptr;

        static inline std::atomic<uint64_t> sequence_ = 0;

        static inline std::atomic<uint64_t> frameCount_ = 0;

        static inline std::atomic<uint64_t> tickCount_ = 0;

        static inline std::chrono::steady_clock::time_point startTime_ = std::chrono::steady_clock::now();

        static const char *ToString(LogLabel label) {
            switch (label) {
                case LogLabel::DEFAULT:
                    return "DEFAULT";
                case LogLabel::WORLD_GEN:
                    return "WORLD_GEN";
                case LogLabel::CHUNK:
                    return "CHUNK";
                case LogLabel::TERRAIN:
                    return "TERRAIN";
            }
            return "UNKNOWN";
        }

        static std::string Resolve(std::string_view key, std::string_view fallback) {
            const auto table = localizer_.load();
            if (table != nullptr) {
                const auto it = table->find(std::string(key));
                if (it != table->end()) {
                    return it->second;
                }
            }
            return std::string(fallback);
        }

        template<typename... Args>
        static std::string Format(std::string_view key, std::string_view fallback, Args &&... args) {
            const std::string templateString = Resolve(key, fallback);
            return fmt::vformat(templateString, fmt::make_format_args(args...));
        }

        static std::string CurrentTime() {
            const auto now = std::chrono::system_clock::now();
            const std::time_t t = std::chrono::system_clock::to_time_t(now);
            std::tm tm{};
#if defined(_WIN32)
            localtime_s(&tm, &t);
#else
            localtime_r(&t, &tm);
#endif
            const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
            std::ostringstream oss;
            oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << '.' << std::setw(3) << std::setfill('0') << ms.count();
            return oss.str();
        }

        static std::string CurrentThreadId() {
            std::ostringstream oss;
            oss << std::this_thread::get_id();
            return oss.str();
        }

        static std::string ElapsedTime() {
            const auto now = std::chrono::steady_clock::now();
            const auto elapsed = now - startTime_;
            const auto secs = std::chrono::duration_cast<std::chrono::seconds>(elapsed);
            const auto us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed - secs);
            std::ostringstream oss;
            oss << '+' << secs.count() << '.' << std::setw(6) << std::setfill('0') << us.count();
            return oss.str();
        }

        static std::string CurrentThreadName() {
            if (!logThreadName_.empty()) {
                return logThreadName_;
            }
            return CurrentThreadId();
        }

        static uint64_t NextSequence() {
            return sequence_.fetch_add(1, std::memory_order_relaxed);
        }

        static std::string Timestamp(std::string_view level, const LogLabel label) {
            std::ostringstream oss;
            oss << "[#" << NextSequence() << "|F:" << frameCount_.load(std::memory_order_relaxed)
                    << "|T:" << tickCount_.load(std::memory_order_relaxed) << '|' << ElapsedTime() << '|'
                    << CurrentTime() << '|' << CurrentThreadName() << '|' << level << '|' << ToString(label) << ']';
            return oss.str();
        }

    public:
        static void SetLocalizer(std::shared_ptr<const LogTable> table) {
            localizer_.store(std::move(table));
        }

        static void IncrementFrameCount() {
            frameCount_.fetch_add(1, std::memory_order_relaxed);
        }

        static void SetTickCount(const uint64_t tick) {
            tickCount_.store(tick, std::memory_order_relaxed);
        }

        static void SetThreadName(const std::string_view name) {
            logThreadName_ = std::string(name);
#if defined(_WIN32)
            const int length = MultiByteToWideChar(CP_UTF8, 0, name.data(), static_cast<int>(name.size()), nullptr, 0);
            std::wstring wide(length, L'\0');
            MultiByteToWideChar(CP_UTF8, 0, name.data(), static_cast<int>(name.size()), wide.data(), length);
            SetThreadDescription(GetCurrentThread(), wide.c_str());
#elif defined(__APPLE__)
            pthread_setname_np(name.data());
#else
            pthread_setname_np(pthread_self(), name.data());
#endif
        }

        static void ClearLocalizer() {
            localizer_.store(nullptr);
        }

        template<typename... Args>
        static void i([[maybe_unused]] const LogLabel label, [[maybe_unused]] std::string_view key,
                      [[maybe_unused]] std::string_view fallback, [[maybe_unused]] Args &&... args) {
#if  !defined(NDEBUG)
            const std::string message = Format(key, fallback, std::forward<Args>(args)...);
#ifdef __ANDROID__
            __android_log_print(ANDROID_LOG_INFO, "GlimmerWorks", "%s %s", Timestamp("i", label).c_str(),
                                message.c_str());
#else
            std::cout << COLOR_INFO << Timestamp("i", label) << " " << message << COLOR_RESET << std::endl;
#endif
#endif
        }

        template<typename... Args>
        static void d([[maybe_unused]] const LogLabel label, [[maybe_unused]] std::string_view key,
                      [[maybe_unused]] std::string_view fallback, [[maybe_unused]] Args &&... args) {
#if  !defined(NDEBUG)
            const std::string message = Format(key, fallback, std::forward<Args>(args)...);
#ifdef __ANDROID__
            __android_log_print(ANDROID_LOG_DEBUG, "GlimmerWorks", "%s %s", Timestamp("d", label).c_str(),
                                message.c_str());
#else
            std::cout << COLOR_DEBUG << Timestamp("d", label) << " " << message << COLOR_RESET << std::endl;
#endif
#endif
        }

        template<typename... Args>
        static void w([[maybe_unused]] const LogLabel label, [[maybe_unused]] const std::source_location sourceLocation,
                      [[maybe_unused]] std::string_view key,
                      [[maybe_unused]] std::string_view fallback, [[maybe_unused]] Args &&... args) {
#if  !defined(NDEBUG)
            const std::string message = Format(key, fallback, std::forward<Args>(args)...);
#ifdef __ANDROID__
            std::ostringstream oss;
            oss << Timestamp("w", label) << " At " << sourceLocation.file_name() << ":" << sourceLocation.line()
                    << " " << message;
            __android_log_print(ANDROID_LOG_WARN, "GlimmerWorks", "%s", oss.str().c_str());
#else
            std::cout << COLOR_WARN;
            std::cout << Timestamp("w", label) << " At " << sourceLocation.file_name() << ":" << sourceLocation.line()
                    << " " << message;
            std::cout << COLOR_RESET << std::endl;
#endif
#endif
        }

        /**
         * Errors applicable to the internal part of the engine.
         * 适用于引擎内部的错误。
         */
        template<typename... Args>
        static void e(const LogLabel label, const std::source_location sourceLocation, std::string_view key,
                      std::string_view fallback, Args &&... args) {
            const std::string message = Format(key, fallback, std::forward<Args>(args)...);
#ifdef __ANDROID__
            std::ostringstream oss;
            oss << Timestamp("e", label) << " At " << sourceLocation.file_name() << ":" << sourceLocation.line()
                    << " " << message;
            __android_log_print(ANDROID_LOG_ERROR, "GlimmerWorks", "%s", oss.str().c_str());
#else
            std::cout << COLOR_ERROR;
            std::cout << Timestamp("e", label) << " At " << sourceLocation.file_name() << ":" << sourceLocation.line()
                    << " " << message;
            std::cout << COLOR_RESET << std::endl;
#endif
#if  !defined(NDEBUG)
            assert(false);
#endif
        }

        /**
         * Used for outputting errors related to data packets and material packages, targeted at players / data packet / material package developers.
         * 用于输出数据包，材质包相关的错误，面向玩家/数据包/材质包开发者。
         */
        template<typename... Args>
        static void publicError(const LogLabel label, const ErrorCode errorCode,
                                const std::source_location sourceLocation, std::string_view key,
                                std::string_view fallback,
                                Args &&... args) {
            const std::string message = Format(key, fallback, std::forward<Args>(args)...);
#ifdef __ANDROID__
            std::ostringstream oss;
            oss << Timestamp("e", label) << " At " << sourceLocation.file_name() << ":" << sourceLocation.line()
                    << " " << message;
            __android_log_print(ANDROID_LOG_ERROR, "GlimmerWorks", "%s", oss.str().c_str());
#else
            std::cout << COLOR_ERROR;
            std::cout << Timestamp("e-" + std::to_string(static_cast<uint32_t>(errorCode)), label) << " At " <<
                    sourceLocation.file_name() << ":" << sourceLocation.line() << " " << message;
            std::cout << COLOR_RESET << std::endl;
#endif
#if  !defined(NDEBUG)
            assert(false);
#endif
        }
    };
}
