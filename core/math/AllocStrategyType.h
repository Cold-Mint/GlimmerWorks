//
// Created by Cold-Mint on 2026/5/27.
//


#pragma once
#include <cstdint>

namespace glimmer
{
    enum class AllocStrategyType: uint8_t
    {
        Forward, // 正序分配
        Backward, // 倒序分配
        Balance, // 均衡分配
        Random // 随机分配
    };
}
