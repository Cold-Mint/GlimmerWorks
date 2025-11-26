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
         * @param worldId Box2dWorldid Box2d世界id
         * @param tileLayerPos The coordinates of the tile layer 瓦片图层的坐标
         * @param chunk  The chunk to add physical bodies to 要添加物理的区块
         */
        static void AttachPhysicsBodyToChunk(b2WorldId worldId, const WorldVector2D& tileLayerPos, Chunk* chunk);

        static b2BodyId CreateStaticBody(b2WorldId worldId, WorldVector2D pos, Vector2DI size);

        static void CreateDynamicTileBody(Chunk* chunk, TileVector2D pos);

        /**
         * Remove physical bodies from the tiles within the block
         * 从区块内的瓦片移除物理体
         * @param chunk The chunk to remove physical bodies from
         */
        static void DetachPhysicsBodyToChunk(Chunk* chunk);
    };
}

#endif //GLIMMERWORKS_CHUNKPHYSICSHELPER_H
