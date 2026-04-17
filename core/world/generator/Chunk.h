//
// Created by Cold-Mint on 2025/11/4.
//

#ifndef GLIMMERWORKS_CHUNK_H
#define GLIMMERWORKS_CHUNK_H
#include <unordered_map>

#include "src/saves/chunk.pb.h"
#include "box2d/id.h"
#include "../../ecs/component/TileLayerComponent.h"
#include "core/Constants.h"
#include "core/world/Tile.h"


namespace glimmer {
    class Chunk {
        TileVector2D position;
        std::unordered_map<TileLayerType, std::array<std::unique_ptr<Tile>, CHUNK_AREA> >
        tiles_;
        std::vector<b2BodyId> attachedBodies_;

        std::vector<std::function<void(Chunk *chunk, int index, Tile *tile, TileLayerType layerType)> >
        setTileCallback_;

        std::vector<std::function<void(Chunk *chunk, TileLayerType layerType, int index,
                                       Tile *oldTile, Tile *newTile)> > replaceTileCallback_;

        void InvokeSetTileCallback(Chunk *chunk, int index, Tile *tile, TileLayerType layerType) const;

        void InvokeReplaceTileCallback(Chunk *chunk, TileLayerType layerType, int index,
                                       Tile *oldTile, Tile *newTile) const;

    public:
        explicit Chunk(const TileVector2D &pos);


        void AddBodyId(b2BodyId bodyId);


        [[nodiscard]] const std::vector<b2BodyId> &GetAttachedBodies();

        void ClearAttachedBodies();

        size_t AddSetTileCallback(
            const std::function<void(Chunk *chunk, int index, Tile *tile, TileLayerType layerType)> &callBack);

        bool RemoveSetTileCallback(long index);

        size_t AddReplaceTileCallback(const std::function<void(Chunk *chunk, TileLayerType layerType,
                                                               int index,
                                                               Tile *oldTile, Tile *newTile)> &callBack);

        bool RemoveReplaceTileCallback(long index);

        /**
         * TileCoordinatesToChunkCoordinates
         * 瓦片坐标转区块顶点坐标
         * @param tileVector2d
         * @return
         */
        [[nodiscard]] static TileVector2D TileCoordinatesToChunkVertexCoordinates(TileVector2D tileVector2d);

        /**
         * TileCoordinatesToChunkRelativeCoordinates
         * 瓦片坐标转区块相对坐标
         * @param tileVector2d
         * @return
         */
        [[nodiscard]] static TileVector2D TileCoordinatesToChunkRelativeCoordinates(TileVector2D tileVector2d);

        void SetTile(TileVector2D pos, std::unique_ptr<Tile> tile);

        void SetTileToLayer(TileVector2D pos, TileLayerType layerType, std::unique_ptr<Tile> tile);

        void SetTileToLayer(int index, TileLayerType layerType, std::unique_ptr<Tile> tile);

        [[nodiscard]] TileVector2D GetPosition() const;

        [[nodiscard]] Tile *GetTile(TileLayerType layerType, int x, int y) const;

        [[nodiscard]] Tile *GetTile(TileLayerType layerType, int index) const;

        [[nodiscard]] Tile *GetTile(TileLayerType layerType, const TileVector2D &tileVector2d) const;

        /**
         * GetTopVisibleTile
         * 获取顶层可见的瓦片。
         * @param layerFilter layerFilter 图层过滤
         * @param tileVector2d tileVector2d 坐标
         * @return
         */
        [[nodiscard]] std::vector<Tile *> GetTopVisibleTiles(uint8_t layerFilter,
                                                             const TileVector2D &tileVector2d) const;

        void ReadChunkMessage(const AppContext *appContext, const ChunkMessage &chunkMessage);

        void WriteChunkMessage(ChunkMessage &chunkMessage);

        WorldVector2D GetStartWorldPosition() const;

        WorldVector2D GetEndWorldPosition() const;

        /**
         * Replace tile
         * 替换瓦片
         * @param layerType layerType 图层
         * @param tileVector2d tileVector2d 瓦片坐标
         * @param newTile newTile 新瓦片
         * @return
         */
        [[nodiscard]] std::unique_ptr<Tile> ReplaceTile(TileLayerType layerType, const TileVector2D &tileVector2d,
                                                        std::unique_ptr<Tile> newTile);
    };
}

#endif //GLIMMERWORKS_CHUNK_H
