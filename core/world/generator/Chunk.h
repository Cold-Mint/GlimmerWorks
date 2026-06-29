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
#include <unordered_map>

#include "TileSnapshot.h"
#include "src/saves/chunk.pb.h"
#include "box2d/id.h"
#include "core/ecs/component/TileLayerComponent.h"
#include "core/Constants.h"
#include "core/math/WorldVector2D.h"
#include "core/world/Tile.h"


namespace glimmer
{
    class Chunk
    {
        TileVector2D position;
        WorldContext* worldContext_;
        std::unordered_map<TileLayerType, std::array<std::shared_ptr<Tile>, CHUNK_AREA>>
        tiles_;

        //Save the tile resource signature for each grid, and rebuild the tile when the signature changes.
        //保存每个格子的瓦片资源签名，并在签名改变时重建瓦片。
        std::unordered_map<TileLayerType, std::array<uint64_t, CHUNK_AREA>>
        tileFingerprint_;
        std::unordered_map<TileLayerType, std::array<std::unique_ptr<TileStateMessage>, CHUNK_AREA>>
        tileState_;
        std::unordered_map<TileLayerType, std::array<std::unique_ptr<TileSnapshot>, CHUNK_AREA>> tileSnapshots_;
        std::vector<b2BodyId> attachedBodies_;
        //chunk fade-in easing animation
        //区块淡入缓动动画
        tweeny::tween<float> chunkFadeInTween_;
        float chunkFadeAlpha_ = 0.0F;
        std::vector<std::function<void(Chunk* chunk, int index, std::shared_ptr<Tile> tile, TileLayerType layerType)>>
        onTileRebuilt_;

        std::vector<std::function<void(Chunk* chunk, TileLayerType layerType, int index,
                                       std::shared_ptr<Tile> oldTile,
                                       std::shared_ptr<Tile> newTile)>> replaceTileCallback_;

        void InvokeReplaceTileCallback(Chunk* chunk, TileLayerType layerType, int index,
                                       const std::shared_ptr<Tile>& oldTile,
                                       const std::shared_ptr<Tile>& newTile) const;

    public:
        explicit Chunk(WorldContext* worldContext, const TileVector2D& pos, const AnimConfig& animConfig);

        void UpdateFadeInAnimation(float delta);

        void AddBodyId(b2BodyId bodyId);

        [[nodiscard]] float GetChunkFadeAlpha() const;

        [[nodiscard]] const std::vector<b2BodyId>& GetAttachedBodies();

        void ClearAttachedBodies();

        size_t AddReplaceTileCallback(const std::function<void(Chunk* chunk, TileLayerType layerType,
                                                               int index,
                                                               std::shared_ptr<Tile> oldTile,
                                                               std::shared_ptr<Tile> newTile)>& callBack);

        bool RemoveReplaceTileCallback(long index);

        /**
         * TileCoordinatesToChunkCoordinates
         * 瓦片坐标转区块顶点坐标
         * @param tileVector2d
         * @return
         */
        [[nodiscard]] static TileVector2D TileCoordinatesToChunkVertexCoordinates(const TileVector2D& tileVector2d);

        /**
         * TileCoordinatesToChunkRelativeCoordinates
         * 瓦片坐标转区块相对坐标
         * @param tileVector2d
         * @return
         */
        [[nodiscard]] static TileVector2D TileCoordinatesToChunkRelativeCoordinates(const TileVector2D& tileVector2d);

        bool CommitTileState(TileLayerType layerType, int index, bool fallback);

        [[nodiscard]] TileStateMessage* GetTileState(TileLayerType layerType, uint8_t index) const;

        [[nodiscard]] TileStateMessage* GetOrCreateTileState(TileLayerType layerType, int index);

        [[nodiscard]] TileVector2D GetPosition() const;

        [[nodiscard]] const Tile* GetTile(TileLayerType layerType, uint8_t index) const;

        [[nodiscard]] std::shared_ptr<Tile> GetTileShared(TileLayerType layerType, uint8_t index) const;


        [[nodiscard]] std::vector<TileSnapshot*> GetTopVisibleTileSnapshots(
            uint8_t layerFilter, uint8_t index) const;

        void ReadChunkMessage(const ChunkMessage& chunkMessage);

        void WriteChunkMessage(ChunkMessage& chunkMessage);

        WorldVector2D GetStartWorldPosition() const;

        WorldVector2D GetEndWorldPosition() const;
    };
}
