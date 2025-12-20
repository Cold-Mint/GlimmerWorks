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
        std::pmr::unordered_map<TileLayerType, std::array<std::array<Tile, CHUNK_SIZE>, CHUNK_SIZE> > tiles_;
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
         * TileCoordinatesToChunkRelativeCoordinates
         * 瓦片坐标转区块相对坐标
         * @param tileVector2d
         * @return
         */
        [[nodiscard]] static TileVector2D TileCoordinatesToChunkRelativeCoordinates(TileVector2D tileVector2d);


        void SetTile(TileVector2D pos, const Tile &tile);

        [[nodiscard]] TileVector2D GetPosition() const;

        [[nodiscard]] const Tile &GetTile(TileLayerType layerType, int x, int y);


        [[nodiscard]] const Tile &GetTile(TileLayerType layerType, TileVector2D tileVector2d);
    };
}

#endif //GLIMMERWORKS_CHUNK_H
