//
// Created by Cold-Mint on 2026/5/19.
//

#pragma once
#include <cstdint>

namespace glimmer {
    enum class TileAnchorType: uint8_t {
        TopLeft,
        TopCenter,
        TopRight,
        CenterLeft,
        Center,
        CenterRight,
        BottomLeft,
        BottomCenter,
        BottomRight,
        Custom,
    };
}
