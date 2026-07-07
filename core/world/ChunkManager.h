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

#include <cstdint>
#include <memory>
#include <unordered_map>

#include "LightBuffer.h"
#include "TileInstancePool.h"
#include "core/ecs/EcsTypes.h"
#include "core/math/Vector2DIHash.h"
#include "generator/Chunk.h"

namespace glimmer
{
    class WorldContext;
    class Tile;

    /**
     * ChunkManager
     * 区块管理器，负责区块的加载、卸载、保存、光照和tile实例池。
     * 从 WorldContext 拆分而来。
     */
    class ChunkManager
    {
        uint32_t chunkSnapshot_ = 0;
        uint32_t lastChunkSnapshot_ = UINT32_MAX;
        std::unordered_map<TileVector2D, std::unique_ptr<Chunk>, Vector2DIHash> chunks_;
        std::unordered_map<TileVector2D, Chunk*, Vector2DIHash> chunksCache_;
        std::unique_ptr<LightBuffer> lightBuffer_ = nullptr;
        std::unique_ptr<TileInstancePool> tileInstancePool_;
        WorldContext* worldContext_ = nullptr;

        /**
         * OnChunkTileChange
         * 当区块内的瓦片改变时
         * @param chunk chunk 区块
         * @param tile tile 瓦片
         * @param layerType layerType 图层类型
         * @param index index 索引
         */
        void OnChunkTileChange(Chunk* chunk, const std::shared_ptr<Tile>& tile, TileLayerType layerType,
                               int index) const;

        void UpdateTileLight(const Chunk* chunk, TileLayerType layerType, int index) const;

        /**
         * Update the lighting for the entire chunk.
         * 更新整个区块的光照。
         * @param chunk
         */
        void UpdateChunkLight(const Chunk* chunk) const;

    public:
        explicit ChunkManager(WorldContext* worldContext);

        /**
        * Load Chunk
        * 加载区块
        * @param position position 位置
        */
        void LoadChunkAt(TileVector2D position);

        /**
         * Unload Chunk
         * 卸载区块
         * @param position position 位置
         */
        void UnloadChunkAt(const TileVector2D& position);

        /**
         * GetChunk
         * 获取指定位置的区块。
         * @param chunkVertex chunkVertex 区块顶点位置
         * @return
         */
        [[nodiscard]] Chunk* GetChunk(TileVector2D chunkVertex);

        /**
         * GetChunks
         * 获取区块
         * @return
         */
        [[nodiscard]] std::unordered_map<TileVector2D, Chunk*, Vector2DIHash>* GetAllChunks();

        /**
         * Determine whether a block at a certain position has been loaded
         * 判断某个位置的区块是否被加载
         * @param position
         * @return
         */
        [[nodiscard]] bool HasChunk(TileVector2D position) const;

        /**
         * SaveChunk
         * 保存某个区块
         * @param position
         */
        [[nodiscard]] bool SaveChunk(TileVector2D position);

        /**
         * Check whether the block exceeds the boundary
         * 检查区块是否超出边界
         * @param position 区块位置 position
         * @return Whether it exceeds the boundary 是否超出边界
         */
        [[nodiscard]] static bool ChunkIsOutOfBounds(TileVector2D position);

        [[nodiscard]] LightBuffer* GetLightingBuffer() const;

        [[nodiscard]] TileInstancePool* GetTileInstancePool() const;
    };
}
