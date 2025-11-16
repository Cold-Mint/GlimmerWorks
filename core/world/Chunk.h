//
// Created by Cold-Mint on 2025/11/4.
//

#ifndef GLIMMERWORKS_CHUNK_H
#define GLIMMERWORKS_CHUNK_H
#include "Tile.h"
#include "../Constants.h"
#include "../math/Vector2DI.h"

namespace glimmer
{
    class Chunk
    {
        TileVector2D position;
        std::array<std::array<Tile, CHUNK_SIZE>, CHUNK_SIZE> tiles_;

    public:
        explicit Chunk(const TileVector2D& pos) : position(pos)
        {
        }

        void SetTile(TileVector2D pos, const Tile& tile);

        [[nodiscard]] TileVector2D GetPosition() const { return position; }

        [[nodiscard]] const Tile& GetTile(const int x, const int y) const { return tiles_[x][y]; }

        Tile& GetTile(const int x, const int y) { return tiles_[x][y]; }

        [[nodiscard]] const Tile& GetTile(TileVector2D tileVector2d) const
        {
            return tiles_[tileVector2d.x][tileVector2d.y];
        }

        Tile& GetTile(const TileVector2D tileVector2d) { return tiles_[tileVector2d.x][tileVector2d.y]; }
    };
}

#endif //GLIMMERWORKS_CHUNK_H
