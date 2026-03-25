//
// Created by coldmint on 2026/3/25.
//

#ifndef GLIMMERWORKS_AUDIOTYPE_H
#define GLIMMERWORKS_AUDIOTYPE_H
#include <cstdint>

namespace glimmer {
    /**
     * AudioType
     * 音效类型
     */
    enum AudioType : uint8_t {
        BGM = 0,
        AMBIENT = 1
    };
}

#endif //GLIMMERWORKS_AUDIOTYPE_H
