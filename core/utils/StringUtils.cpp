//
// Created by coldmint on 2026/4/1.
//

#include "StringUtils.h"

#include <cstdint>
#include <iomanip>
#include <ios>

std::string glimmer::StringUtils::ToSafeSaveName(const std::string &utf8Str) {
    std::uint32_t hash = 0x811C9DC5;
    for (unsigned char c : utf8Str) {
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
