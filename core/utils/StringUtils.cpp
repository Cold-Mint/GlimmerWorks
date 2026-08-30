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
#include "StringUtils.h"

#include <cctype>
#include <chrono>
#include <cstring>
#include <iomanip>
#include <ios>
#include <sstream>

#include "blake3.h"
#include "core/config/Constants.h"
#include "fmt/color.h"

std::string glimmer::StringUtils::ToSafeSaveName(const std::string &utf8Str) {
    std::ostringstream oss;
    oss << "save_";
    oss << StringToUint64Blake3(utf8Str);
    return oss.str();
}

std::string glimmer::StringUtils::SanitizeFileName(const std::string &name) {
    std::string result = name;
    for (char &c: result) {
        if (const auto uc = static_cast<unsigned char>(c); std::isalnum(uc) == 0 && c != '.' && c != '_' && c != '-') {
            c = '_';
        }
    }
    return result;
}

uint64_t glimmer::StringUtils::StringToUint64Blake3(const std::string &string) {
    blake3_hasher hasher;
    blake3_hasher_init(&hasher);
    blake3_hasher_update(&hasher, string.data(), string.size());
    uint8_t singleHash[BLAKE3_OUT_LEN];
    blake3_hasher_finalize(&hasher, singleHash, BLAKE3_OUT_LEN);
    uint64_t res = 0;
    std::memcpy(&res, singleHash, sizeof(uint64_t));
    return res;
}

std::array<uint8_t, BLAKE3_OUT_LEN> glimmer::StringUtils::StringToFullBlake3(const std::string &string) {
    blake3_hasher hasher;
    blake3_hasher_init(&hasher);
    blake3_hasher_update(&hasher, string.data(), string.size());
    std::array<uint8_t, 32> out{};
    blake3_hasher_finalize(&hasher, out.data(), BLAKE3_OUT_LEN);
    return out;
}

bool glimmer::StringUtils::IsInteger(const std::string &str) {
    if (str.empty()) {
        return false;
    }

    size_t checkStart = 0;
    if (str[0] == '-') {
        if (str.size() == 1) {
            return false;
        }
        checkStart = 1;
    }
    for (size_t i = checkStart; i < str.size(); ++i) {
        auto c = static_cast<unsigned char>(str[i]);
        if (!std::isdigit(c)) {
            return false;
        }
    }
    return true;
}

uint64_t glimmer::StringUtils::StringToUint64(const std::string &string) {
    return std::hash<std::string>{}(string);
}

std::span<const std::byte> glimmer::StringUtils::StringToByteData(const std::string_view string) {
    return {
        reinterpret_cast<const std::byte *>(string.data()), string.size()
    };
}

std::string glimmer::StringUtils::MakeRawText(const std::string_view string) {
    if (string.empty()) {
        return "";
    }
    if (string.starts_with(RAW_TEXT_PREFIX)) {
        return std::string(string);
    }
    return fmt::format("{}{}", RAW_TEXT_PREFIX, string);
}

std::string glimmer::StringUtils::MakeTextureUrl(const std::string_view resourceKey) {
    if (resourceKey.empty()) {
        return "";
    }
    if (resourceKey.starts_with(TEXTURE_PREFIX)) {
        return std::string(resourceKey);
    }
    return fmt::format("{}{}", TEXTURE_PREFIX, resourceKey);
}

std::optional<std::string> glimmer::StringUtils::StreamToString(const std::istream *stream) {
    if (!stream) {
        return std::nullopt;
    }
    if (stream->fail()) {
        return std::nullopt;
    }
    std::ostringstream oss;
    oss << stream->rdbuf();
    if (oss.fail()) {
        return std::nullopt;
    }
    return oss.str();
}

std::string glimmer::StringUtils::GetScreenshotFileName() {
    auto now = std::chrono::system_clock::now();
    auto t = std::chrono::system_clock::to_time_t(now);
    std::tm tm;
#if defined(_WIN32)
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    std::ostringstream oss;
    oss << "screenshot_"
            << std::put_time(&tm, "%Y%m%d_%H%M%S") << "."
            << TEXTURE_FORMAT;
    return oss.str();
}

void glimmer::StringUtils::ReplaceAll(std::string &str, const std::string_view from, const std::string_view to) {
    if (from.empty()) {
        return;
    }
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}
