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

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

#include "Dimension.h"
#include "LightBuffer.h"
#include "TileInstancePool.h"
#include "core/math/Vector2DIHash.h"
#include "core/mod/ResourceRefHash.h"
#include "generator/Chunk.h"
#include "generator/ChunkLoader.h"
#include "generator/TerrainGenerator.h"

namespace glimmer {
    class WorldContext;
    class Tile;

    /**
     * ChunkManager
     * 区块管理器，负责区块的加载、卸载、保存、光照和tile实例池。
     * 从 WorldContext 拆分而来。
     */
    class ChunkManager {
        TerrainGenerator *terrainGenerator_ = nullptr;
        ChunkLoader *chunkLoader_ = nullptr;

        /**
         * Save the map of the block.
         * 保存区块的Map。
         *
         * Key is a pointer for referencing dimension resources.
         * Key为维度资源引用的指针。
         */
        std::unordered_map<ResourceRef, std::unordered_map<TileVector2D, std::unique_ptr<Chunk>,
            Vector2DIHash>, ResourceRefHash>
        dimensionMap_;
        std::unique_ptr<LightBuffer> lightBuffer_ = nullptr;
        std::unique_ptr<TileInstancePool> tileInstancePool_ = nullptr;
        WorldContext *worldContext_ = nullptr;

        /**
         * OnChunkTileChange
         * 当区块内的瓦片改变时
         * @param chunk chunk 区块
         * @param tile tile 瓦片
         * @param layerType layerType 图层类型
         * @param index index 索引
         */
        void OnChunkTileChange(Chunk *chunk, const std::shared_ptr<Tile> &tile, TileLayerType layerType,
                               int index) const;

        void UpdateTileLight(const Chunk *chunk, TileLayerType layerType, int index) const;

        /**
         * Update the lighting for the entire chunk.
         * 更新整个区块的光照。
         * @param chunk
         */
        void UpdateChunkLight(const Chunk *chunk) const;

    public:
        explicit ChunkManager(WorldContext *worldContext);

        /**
        * Load Chunk
        * 加载区块
        * @param maxChunksOccupiedByStructure maxChunksOccupiedByStructure 结构占用的最大区块数
        * @param dimensionRef dimensionRef 维度资源引用
        * @param position position 位置
        */
        void LoadChunkAt(uint32_t maxChunksOccupiedByStructure, const ResourceRef &dimensionRef,
                         const TileVector2D &position);

        /**
         * Unload Chunk
         * 卸载区块
         * @param dimensionRef dimensionRef
         * @param position position 位置
         */
        void UnloadChunkAt(const ResourceRef &dimensionRef, const TileVector2D &position);

        /**
         * GetChunk
         * 获取指定位置的区块。
         * @param dimensionRef dimensionRef 维度资源引用
         * @param position position 区块顶点位置
         * @return
         */
        [[nodiscard]] Chunk *GetChunk(const ResourceRef &dimensionRef, const TileVector2D &position) const;

        /**
         * Determine whether a block at a certain position has been loaded
         * 判断某个位置的区块是否被加载
         * @param dimensionRef dimensionRef 维度资源引用
         * @param position position 位置
         * @return
         */
        [[nodiscard]] bool HasChunk(const ResourceRef &dimensionRef, const TileVector2D &position) const;

        /**
         * SaveChunk
         * 保存某个区块
         * @param dimensionRef
         * @param position
         */
        [[nodiscard]] bool SaveChunk(const ResourceRef &dimensionRef, const TileVector2D &position) const;

        /**
         * SaveAllChunk
         * 保存所有的区块
         * @return 成功保存了多少个区块
         */
        size_t SaveAllChunk();

        /**
         * GetChunkCount
         * 获取某个维度的区块数
         * @param dimensionRef
         * @return
         */
        size_t GetLoadedChunkCount(const ResourceRef &dimensionRef) const;

        const std::unordered_map<TileVector2D, std::unique_ptr<Chunk>,
            Vector2DIHash> *GetLoadedChunks(const ResourceRef &dimensionRef) const;


        /**
         * Check whether the block exceeds the boundary
         * 检查区块是否超出边界
         * @param position 区块位置 position
         * @return Whether it exceeds the boundary 是否超出边界
         */
        [[nodiscard]] static bool ChunkIsOutOfBounds(const TileVector2D &position);

        [[nodiscard]] LightBuffer *GetLightingBuffer() const;

        [[nodiscard]] TileInstancePool *GetTileInstancePool() const;
    };
}
