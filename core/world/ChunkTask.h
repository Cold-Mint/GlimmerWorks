//
// Created by coldmint on 2026/5/13.
//

#ifndef GLIMMERWORKS_CHUNKTASK_H
#define GLIMMERWORKS_CHUNKTASK_H
#include "ChunkType.h"
#include "core/ISignable.h"
#include "core/ecs/component/TileLayerComponent.h"

namespace glimmer {
    class ChunkTask : public ISignable {
        ChunkType chunkType_ = None;
        TileVector2D chunkVertexCoordinates_;
        uint32_t distance_ = 0;

    public:
        ChunkTask(ChunkType chunkType, TileVector2D chunkVertexCoordinates);

        [[nodiscard]] ChunkType GetChunkType() const;

        [[nodiscard]] uint32_t GetDistance() const;

        void SetOrigin(TileVector2D origin);

        [[nodiscard]] const TileVector2D &GetChunkVertexCoordinates() const;

        [[nodiscard]] uint64_t GetSignature() const override;
    };
}

#endif //GLIMMERWORKS_CHUNKTASK_H
