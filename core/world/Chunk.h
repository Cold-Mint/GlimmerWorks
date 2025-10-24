//
// Created by Cold-Mint on 2025/10/24.
//

#ifndef GLIMMERWORKS_CHUNK_H
#define GLIMMERWORKS_CHUNK_H

#include <string>
#include <vector>

#include "WorldContext.h"
#include "../math/Vector2D.h"

namespace Glimmer {
    /**
     * Chunk
     * 区块
     */
    struct Chunk {
        Vector2D origin;
        WorldContext *worldContext;
        std::vector<std::vector<std::string> > tileIDs;

        explicit Chunk(const Vector2D origin, WorldContext *worldContext) : origin(origin), worldContext(worldContext) {
            tileIDs = std::vector(
                CHUNK_SIZE, std::vector(CHUNK_SIZE, TILE_ID_AIR));
        }

        /**
         * Place a tile
         * 放置瓦片
         * @param position position((Relative to the origin of the block) 位置（相对于区块的原点）
         * @param tileId tileId 瓦片id
         */
        void place(Vector2D position, const std::string &tileId);

        /**
         * Write the chunk data to the file
         * 将区块数据写入文件
         */
        void save();


        /**
         * Load chunk (load from file)
         * 加载区块（从文件加载）
         */
        void load();
    };
}

#endif //GLIMMERWORKS_CHUNK_H
