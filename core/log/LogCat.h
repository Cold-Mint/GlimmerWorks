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
#include <iostream>
#include <memory>
#include <source_location>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <fmt/format.h>
#ifdef __ANDROID__
#include <android/log.h>
#endif
constexpr const char *COLOR_RESET = "\o{33}[0m";
constexpr const char *COLOR_INFO = "\o{33}[32m";
constexpr const char *COLOR_DEBUG = "\o{33}[36m";
constexpr const char *COLOR_WARN = "\o{33}[33m";
constexpr const char *COLOR_ERROR = "\o{33}[31m";

namespace glimmer {
    class LogCat {
        using LogTable = std::unordered_map<std::string, std::string>;

        static inline std::atomic<std::shared_ptr<const LogTable> > localizer_ = nullptr;

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

    public:
        static void SetLocalizer(std::shared_ptr<const LogTable> table) {
            localizer_.store(std::move(table));
        }

        static void ClearLocalizer() {
            localizer_.store(nullptr);
        }

        template<typename... Args>
        static void i([[maybe_unused]] std::string_view key, [[maybe_unused]] std::string_view fallback,
                      [[maybe_unused]] Args &&... args) {
#if  !defined(NDEBUG)
            const std::string message = Format(key, fallback, std::forward<Args>(args)...);
#ifdef __ANDROID__
            __android_log_print(ANDROID_LOG_INFO, "GlimmerWorks", "%s", message.c_str());
#else
            std::cout << COLOR_INFO << "[i] " << message << COLOR_RESET << std::endl;
#endif
#endif
        }

        template<typename... Args>
        static void d([[maybe_unused]] std::string_view key, [[maybe_unused]] std::string_view fallback,
                      [[maybe_unused]] Args &&... args) {
#if  !defined(NDEBUG)
            const std::string message = Format(key, fallback, std::forward<Args>(args)...);
#ifdef __ANDROID__
            __android_log_print(ANDROID_LOG_DEBUG, "GlimmerWorks", "%s", message.c_str());
#else
            std::cout << COLOR_DEBUG << "[d] " << message << COLOR_RESET << std::endl;
#endif
#endif
        }

        template<typename... Args>
        static void w([[maybe_unused]] const std::source_location sourceLocation, [[maybe_unused]] std::string_view key,
                      [[maybe_unused]] std::string_view fallback, [[maybe_unused]] Args &&... args) {
#if  !defined(NDEBUG)
            const std::string message = Format(key, fallback, std::forward<Args>(args)...);
#ifdef __ANDROID__
            std::ostringstream oss;
            oss << "[w] At " << sourceLocation.file_name() << ":" << sourceLocation.line() << " " << message;
            __android_log_print(ANDROID_LOG_WARN, "GlimmerWorks", "%s", oss.str().c_str());
#else
            std::cout << COLOR_WARN;
            std::cout << "[w] At " << sourceLocation.file_name() << ":" << sourceLocation.line() << " " << message;
            std::cout << COLOR_RESET << std::endl;
#endif
#endif
        }

        template<typename... Args>
        static void e(const std::source_location sourceLocation, std::string_view key, std::string_view fallback,
                      Args &&... args) {
            const std::string message = Format(key, fallback, std::forward<Args>(args)...);
#ifdef __ANDROID__
            std::ostringstream oss;
            oss << "[e] At " << sourceLocation.file_name() << ":" << sourceLocation.line() << " " << message;
            __android_log_print(ANDROID_LOG_ERROR, "GlimmerWorks", "%s", oss.str().c_str());
#else
            std::cout << COLOR_ERROR;
            std::cout << "[e] At " << sourceLocation.file_name() << ":" << sourceLocation.line() << " " << message;
            std::cout << COLOR_RESET << std::endl;
#endif
#if  !defined(NDEBUG)
            assert(false);
#endif
        }
    };
}
