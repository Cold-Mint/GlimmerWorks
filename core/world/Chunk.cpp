//
// Created by Cold-Mint on 2025/11/4.
//

#include "Chunk.h"


void glimmer::Chunk::SetTile(const TileVector2D pos, const Tile& tile)
{
    tiles_[pos.x][pos.y] = tile;
}
