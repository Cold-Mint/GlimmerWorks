//
// Created by coldmint on 2026/5/13.
//

#ifndef GLIMMERWORKS_CHUNKTASK_H
#define GLIMMERWORKS_CHUNKTASK_H
#include "ChunkType.h"
#include "core/ecs/component/TileLayerComponent.h"

namespace glimmer {
    struct ChunkTask {
        ChunkType chunkType = None;
        TileVector2D tileVector2D;

        uint64_t GetId() const;
    };


}

#endif //GLIMMERWORKS_CHUNKTASK_H
