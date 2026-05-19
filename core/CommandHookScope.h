//
// Created by coldmint on 2026/5/1.
//

#pragma once
#include <cstdint>

namespace glimmer {
    enum class CommandHookScope : uint8_t {
        CONFIG,
        /**
         * The scope of effect that is only saved in memory
         * 仅内存中保存的作用范围
         */
        SESSION,
    };
}
