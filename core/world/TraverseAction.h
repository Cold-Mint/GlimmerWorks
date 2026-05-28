//
// Created by Cold-Mint on 2026/4/11.
//

#pragma once
#include <cstdint>

namespace glimmer {
    enum class TraverseAction : uint8_t {
        Continue = 0, // 继续
        SkipDirection = 1, // 跳过当前方向
        StopAll = 2, // 停止全部遍历
    };
}
