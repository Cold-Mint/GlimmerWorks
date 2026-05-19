//
// Created by Cold-Mint on 2026/2/19.
//
#pragma once
#include <cstdint>
#include <string>

#include "tweeny.h"

namespace glimmer {
    struct GameUIMessage {
        std::string text;

        uint64_t createTime;
        uint64_t expireTime;

        float alpha = 0.0F;

        tweeny::tween<float> tween;

        GameUIMessage(const std::string &t, uint64_t now);
    };
}

