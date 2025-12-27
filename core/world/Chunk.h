//
// Created by Cold-Mint on 2025/11/4.
//

#ifndef GLIMMERWORKS_CHUNK_H
#define GLIMMERWORKS_CHUNK_H

#include "Tile.h"
#include "../Constants.h"
#include "../math/Vector2DI.h"
#include "box2d/id.h"

namespace glimmer {
    class Chunk {
        TileVector2D position;
        std::pmr::unordered_map<TileLayerType, std::array<std::array<std::unique_ptr<Tile>, CHUNK_SIZE>, CHUNK_SIZE> >
        tiles_;
        std::vector<b2BodyId> attachedBodies_;

        static int TileToChunk(int tileCoord);

    public:
        explicit Chunk(const TileVector2D &pos) : position(pos) {
        }


        void AddBodyId(b2BodyId bodyId);


        [[nodiscard]] const std::vector<b2BodyId> &GetAttachedBodies();

        void ClearAttachedBodies();

        /**
     * TileCoordinatesToChunkCoordinates
     * 瓦片坐标转区块顶点坐标
     * @param tileVector2d
     * @return
     */
        [[nodiscard]] static TileVector2D TileCoordinatesToChunkVertexCoordinates(TileVector2D tileVector2d);

        /**
         * Return the block where it is located based on the given coordinates
         * 根据给定的坐标返回其所在区块
         * @param chunks All the block information 所有的区块信息
         * @param tileVector2d tileVector2d 坐标
         * @return
         */
        static Chunk *GetChunkByTileVector2D(
            std::unordered_map<TileVector2D, Chunk *, Vector2DIHash> *chunks,
            TileVector2D tileVector2d);

        /**
         * TileCoordinatesToChunkRelativeCoordinates
         * 瓦片坐标转区块相对坐标
         * @param tileVector2d
         * @return
         */
        [[nodiscard]] static TileVector2D TileCoordinatesToChunkRelativeCoordinates(TileVector2D tileVector2d);

        void SetTile(TileVector2D pos, std::unique_ptr<Tile> tile);

        [[nodiscard]] TileVector2D GetPosition() const;

        [[nodiscard]] Tile *GetTile(TileLayerType layerType, int x, int y);

        [[nodiscard]] Tile *GetTile(TileLayerType layerType, const TileVector2D &tileVector2d);

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
