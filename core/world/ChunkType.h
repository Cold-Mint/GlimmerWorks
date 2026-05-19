//
// Created by coldmint on 2026/5/13.
//

#pragma once
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