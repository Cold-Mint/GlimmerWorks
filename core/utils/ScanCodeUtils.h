//
// Created by Cold-Mint on 2026/5/2.
//

#pragma once
#include <string>
#include <unordered_map>
#include <vector>

#include "SDL3/SDL_scancode.h"

namespace glimmer {
    class ScanCodeUtils {
        static std::unordered_map<std::string, SDL_Scancode> stringToScanMap_;
        static std::vector<std::string> cachedKeys_;

    public:
        [[nodiscard]] static SDL_Scancode StringToScanCode(const std::string &key);

        [[nodiscard]] static bool ContainsKey(const std::string &key);

        [[nodiscard]] static const std::vector<std::string> &GetAllScanCodeKeys();
    };
}
