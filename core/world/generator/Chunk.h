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
#include <unordered_map>
#include <vector>

#include "TerrainResult.h"
#include "TileSnapshot.h"
#include "src/saves/chunk.pb.h"
#include "box2d/id.h"
#include "core/ecs/component/TileLayerComponent.h"
#include "core/config/Constants.h"
#include "core/math/WorldVector2D.h"
#include "core/world/Tile.h"


namespace glimmer {
    class Chunk {
        TileVector2D position_;
        WorldContext *worldContext_;
        std::unordered_map<TileLayerType, std::array<std::shared_ptr<Tile>, CHUNK_AREA> >
        tiles_;

        //Save the tile resource signature for each grid, and rebuild the tile when the signature changes.
        //保存每个格子的瓦片资源签名，并在签名改变时重建瓦片。
        std::unordered_map<TileLayerType, std::array<uint64_t, CHUNK_AREA> >
        tileFingerprint_;
        std::unordered_map<TileLayerType, std::array<std::unique_ptr<TileStateMessage>, CHUNK_AREA> >
        tileState_;
        std::unordered_map<TileLayerType, std::array<std::unique_ptr<TileSnapshot>, CHUNK_AREA> > tileSnapshots_;
        std::vector<b2BodyId> attachedBodies_;

        //The terrain data that this chunk depends on. Holding the shared_ptr keeps the terrain alive and allows
        //neighboring chunks to reuse it until this chunk is unloaded.
        //此区块依赖的地形数据。持有 shared_ptr 使地形保持存活，直到此区块卸载前可被邻近区块复用。
        std::vector<std::shared_ptr<TerrainResult> > dependencyTerrain_;

        std::vector<std::function<void(Chunk *chunk, int index, std::shared_ptr<Tile> tile, TileLayerType layerType)> >
        onTileRebuilt_;

        std::vector<std::function<void(Chunk *chunk, TileLayerType layerType, int index,
                                       std::shared_ptr<Tile> oldTile,
                                       std::shared_ptr<Tile> newTile)> > replaceTileCallback_;

        void InvokeReplaceTileCallback(Chunk *chunk, TileLayerType layerType, int index,
                                       const std::shared_ptr<Tile> &oldTile,
                                       const std::shared_ptr<Tile> &newTile) const;

        static void WriteTileStatesToMessage(
            const std::array<std::unique_ptr<TileStateMessage>, CHUNK_AREA> &tileStates,
            TileStateArrayMessage &layerMessage);

    public:
        explicit Chunk(WorldContext *worldContext, const TileVector2D &pos);

        /**
         * SetDependencyTerrain
         * 设置此区块依赖的地形数据，持有引用以保持其存活直至区块卸载。
         */
        void SetDependencyTerrain(std::vector<std::shared_ptr<TerrainResult> > dependencyTerrain);

        void AddBodyId(b2BodyId bodyId);

        [[nodiscard]] const std::vector<b2BodyId> &GetAttachedBodies();

        void ClearAttachedBodies();

        size_t AddReplaceTileCallback(const std::function<void(Chunk *chunk, TileLayerType layerType,
                                                               int index,
                                                               std::shared_ptr<Tile> oldTile,
                                                               std::shared_ptr<Tile> newTile)> &callBack);

        bool RemoveReplaceTileCallback(long index);

        /**
         * TileCoordinatesToChunkCoordinates
         * 瓦片坐标转区块顶点坐标
         * @param tileVector2d
         * @return
         */
        [[nodiscard]] static TileVector2D TileCoordinatesToChunkVertexCoordinates(const TileVector2D &tileVector2d);

        /**
         * TileCoordinatesToChunkRelativeCoordinates
         * 瓦片坐标转区块相对坐标
         * @param tileVector2d
         * @return
         */
        [[nodiscard]] static TileVector2D TileCoordinatesToChunkRelativeCoordinates(const TileVector2D &tileVector2d);

        bool CommitTileState(BreakSource breakSource, TileLayerType layerType, int index, bool fallback);

        /**
         * PlaceTile
         * 放置瓦片
         *
         * Writes a tile resource into the tile state at the given index (resource ref, width/height,
         * place source, offset, growth state) and commits the change.
         * 将瓦片资源写入指定索引处的瓦片状态（资源引用、宽高、放置来源、偏移、生长状态）并提交变更。
         *
         * @param layerType layerType 目标图层
         * @param index index 区块内索引
         * @param resourceRef resourceRef 瓦片资源引用
         * @param tileResource tileResource 瓦片资源
         * @param breakSource breakSource 破坏来源（提交时触发旧瓦片 OnBreak）
         * @param placeSource placeSource 放置来源
         * @param offsetX offsetX 偏移X
         * @param offsetY offsetY 偏移Y
         * @param fallback fallback 提交时是否允许 fallback 瓦片
         * @return Whether the tile was placed successfully 是否成功放置
         */
        bool PlaceTile(TileLayerType layerType, int index, const ResourceRef &resourceRef,
                       const TileResource *tileResource, BreakSource breakSource, PlaceSourceMessage placeSource,
                       int offsetX, int offsetY, bool fallback);

        [[nodiscard]] TileStateMessage *GetTileState(TileLayerType layerType, uint8_t index) const;

        [[nodiscard]] TileStateMessage *GetOrCreateTileState(TileLayerType layerType, int index);

        /**
         * Initialize the growth state of a tile state to its default values for a freshly placed tile.
         * 将瓦片状态的生长相关字段初始化为"刚放置"的默认值。
         * @param msg 目标瓦片状态
         * @param tileResource tileResource 瓦片资源
         * @param tick 当前全局 tick
         */
        static void InitGrowthState(TileStateMessage *msg, const TileResource *tileResource, uint64_t tick);

        [[nodiscard]] TileVector2D GetPosition() const;

        [[nodiscard]] const Tile *GetTile(TileLayerType layerType, uint8_t index) const;

        [[nodiscard]] std::shared_ptr<Tile> GetTileShared(TileLayerType layerType, uint8_t index) const;


        [[nodiscard]] std::vector<TileSnapshot *> GetTopVisibleTileSnapshots(
            std::byte layerFilter, uint8_t index) const;

        void ReadChunkMessage(const ChunkMessage &chunkMessage);

        void WriteChunkMessage(ChunkMessage &chunkMessage);

        WorldVector2D GetStartWorldPosition() const;

        WorldVector2D GetEndWorldPosition() const;
    };
}
