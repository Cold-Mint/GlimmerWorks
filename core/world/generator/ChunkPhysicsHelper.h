/*
 * Copyright (C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * 版权(C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * 本程序是自由软件：你可以遵照自由软件基金会出版的GNU Affero通用公共许可证条款来重新分发和修改它
 * 该许可证的第3版，或者（由你选择）任何后续版本。
 *
 * 本程序的发布目的是希望它能有用，但没有任何担保；甚至没有适销性或特定用途适用性的默示担保。
 * 有关详细细节，请参阅GNU Affero通用公共许可证。
 *
 * 你应该已经收到一份GNU Affero通用公共许可证的副本。如果没有，请查阅<https://www.gnu.org/licenses/>。
 */
#pragma once
#include <vector>

#include "Chunk.h"
#include "core/math/ChunkRelativeVector2D.h"
#include "core/math/Vector2DI.h"

namespace glimmer {
    class WorldContext;

    /**
     * A rectangular region of static tiles within a chunk that should be merged into a single physics body.
     * 区块内需要合并为单个物理体的一段静态瓦片矩形区域。
     */
    struct ChunkPhysicsRect {
        int x;
        int y;
        int width;
        int height;
    };

    class ChunkPhysicsHelper {
        static std::vector<bool> CollectStaticTiles(const Chunk *chunk);

        static Vector2DI FindRectSize(int startX, int startY,
                                      const std::vector<bool> &isStaticTile,
                                      std::vector<bool> &visited);

        static void MarkVisited(int startX, int startY, int width, int height,
                                std::vector<bool> &visited);

        /**
         * Compute the list of static tile rectangles for a chunk.
         * Runs on the caller thread and only reads chunk tile data.
         * 计算区块的静态瓦片矩形列表。在调用线程执行，只读取区块瓦片数据。
         */
        static std::vector<ChunkPhysicsRect> CollectRects(const std::shared_ptr<Chunk> &chunk);

        static b2BodyId CreateStaticBody(b2WorldId worldId, const WorldVector2D &pos, const Vector2DI &size);

        static void CreateBodyForRect(b2WorldId worldId, Chunk *chunk, const ChunkPhysicsRect &rect);

        /**
         * Destroy all physics bodies currently attached to a chunk.
         * Runs on the physics thread.
         * 销毁当前附加到区块的所有物理体。在物理线程执行。
         */
        static void DetachPhysicsBodyToChunkNow(const std::shared_ptr<Chunk> &chunk);

        /**
         * Create physics bodies for a chunk from precomputed rectangles.
         * Runs on the physics thread.
         * 根据预计算的矩形为区块创建物理体。在物理线程执行。
         */
        static void AttachPhysicsBodyToChunkNow(b2WorldId worldId, const std::shared_ptr<Chunk>& chunk,
                                                const std::vector<ChunkPhysicsRect> &rects);

    public:
        /**
         * Schedule attaching physics bodies to the tiles within the chunk.
         * 调度为区块内的瓦片添加物理体。
         * @param worldContext worldContext 世界上下文
         * @param chunk chunk 要添加物理的区块
         */
        static void AttachPhysicsBodyToChunk(const WorldContext *worldContext, const std::shared_ptr<Chunk> &chunk);

        /**
         * Schedule removing physics bodies from the tiles within the chunk.
         * 调度从区块内的瓦片移除物理体。
         * @param worldContext worldContext 世界上下文
         * @param chunk chunk 要移除物理体的区块
         */
        static void DetachPhysicsBodyToChunk(const WorldContext *worldContext, const std::shared_ptr<Chunk> &chunk);

        /**
         * Schedule updating the physics collision of the chunk.
         * 调度更新区块的物理碰撞。
         * @param worldContext worldContext 世界上下文
         * @param chunk chunk 要更新的区块
         */
        static void UpdatePhysicsBodyToChunk(const WorldContext *worldContext, const std::shared_ptr<Chunk> &chunk);
    };
}
