//
// Created by coldmint on 2026/5/2.
//

#pragma once
#include <string>

namespace glimmer {
    class UUIDUtils {
    public:
        /**
         * Generate
         * 生成UUID
         * @return
         */
        [[nodiscard]] static std::string Generate();
    };
}
