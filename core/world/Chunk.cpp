//
// Created by Cold-Mint on 2025/11/4.
//

#include "Chunk.h"


void glimmer::Chunk::AddBodyId(b2BodyId bodyId) { attachedBodies_.emplace_back(bodyId); }

const std::vector<b2BodyId> &glimmer::Chunk::GetAttachedBodies() {
    return attachedBodies_;
}

void glimmer::Chunk::ClearAttachedBodies() {
    attachedBodies_.clear();
}

TileVector2D glimmer::Chunk::TileCoordinatesToChunkVertexCoordinates(const TileVector2D tileVector2d) {
    return {
        TileToChunk(tileVector2d.x),
        TileToChunk(tileVector2d.y)
    };
}

TileVector2D glimmer::Chunk::TileCoordinatesToChunkRelativeCoordinates(const TileVector2D tileVector2d) {
    const int chunkX = TileToChunk(tileVector2d.x);
    const int chunkY = TileToChunk(tileVector2d.y);

    return {
        tileVector2d.x - chunkX,
        tileVector2d.y - chunkY
    };
}

int glimmer::Chunk::TileToChunk(const int tileCoord) {
    if (tileCoord >= 0) {
        return tileCoord / CHUNK_SIZE * CHUNK_SIZE;
    }
    return (tileCoord - CHUNK_SIZE + 1) / CHUNK_SIZE * CHUNK_SIZE;
}

void glimmer::Chunk::SetTile(const TileVector2D pos, const Tile &tile) {
    tiles_[tile.layerType][pos.x][pos.y] = tile;
}

TileVector2D glimmer::Chunk::GetPosition() const {
    return position;
}

const glimmer::Tile &glimmer::Chunk::GetTile(const TileLayerType layerType, const int x, const int y) {
    return tiles_[layerType][x][y];
}

const glimmer::Tile &glimmer::Chunk::GetTile(const TileLayerType layerType, const TileVector2D tileVector2d) {
    return tiles_[layerType][tileVector2d.x][tileVector2d.y];
}
