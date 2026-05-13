//
// Created by coldmint on 2026/5/13.
//

#ifndef GLIMMERWORKS_CHUNKTYPE_H
#define GLIMMERWORKS_CHUNKTYPE_H
#include <cstdint>

namespace glimmer {
    enum ChunkType : uint8_t {
        None,
        LoadTerrain,
        LoadChunk,
        UnloadChunk,
        UnloadTerrain
    };
}

#endif //GLIMMERWORKS_CHUNKTYPE_H
