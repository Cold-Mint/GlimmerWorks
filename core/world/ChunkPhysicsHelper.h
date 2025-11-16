//
// Created by Cold-Mint on 2025/11/9.
//

#ifndef GLIMMERWORKS_CHUNKPHYSICSHELPER_H
#define GLIMMERWORKS_CHUNKPHYSICSHELPER_H
#include "Chunk.h"
#include "box2d/id.h"

namespace glimmer
{
    class ChunkPhysicsHelper
    {
    public:
        /**
         * Add physical bodies to the tiles within the block
         * 为区块内的瓦片添加物理体
         * @param chunk The chunk to add physical bodies to
         */
        void AttachPhysicsBodyToChunk(b2WorldId worldId, const WorldVector2D& tileLayerPos, Chunk* chunk);

        void CreateStaticBody(b2WorldId worldId, WorldVector2D pos, Vector2DI size);

        void CreateDynamicTileBody(Chunk* chunk, TileVector2D pos);

        /**
         * Remove physical bodies from the tiles within the block
         * 从区块内的瓦片移除物理体
         * @param chunk The chunk to remove physical bodies from
         */
        void DetachPhysicsBodyToChunk(Chunk* chunk);
    };
}

#endif //GLIMMERWORKS_CHUNKPHYSICSHELPER_H
