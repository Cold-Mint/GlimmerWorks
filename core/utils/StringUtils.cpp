//
// Created by coldmint on 2026/4/1.
//

#include "StringUtils.h"

#include <cstdint>
#include <iomanip>
#include <ios>
#include <sstream>

std::string glimmer::StringUtils::ToSafeSaveName(const std::string &utf8Str) {
    std::uint32_t hash = 0x811C9DC5;
    for (unsigned char c: utf8Str) {
        constexpr uint32_t fnvPrime = 0x01000193;
        hash ^= static_cast<uint32_t>(c);
        hash *= fnvPrime;
    }
    std::ostringstream oss;
    oss << "save_"
            << std::hex
            << std::setw(8) << std::setfill('0')
            << hash;

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