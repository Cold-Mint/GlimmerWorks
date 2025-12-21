//
// Created by Cold-Mint on 2025/11/4.
//

#include "Chunk.h"

#include "../log/LogCat.h"


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

glimmer::Chunk *glimmer::Chunk::GetChunkByTileVector2D(
    std::unordered_map<TileVector2D, Chunk *, Vector2DIHash> *chunks, TileVector2D tileVector2d) {
    const TileVector2D vertexCoordinate = TileCoordinatesToChunkVertexCoordinates(tileVector2d);
    const auto it = chunks->find(vertexCoordinate);
    if (it == chunks->end()) {
        return nullptr;
    }
    return it->second;
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

void glimmer::Chunk::SetTile(const TileVector2D pos, std::unique_ptr<Tile> tile) {
    tiles_[tile->layerType][pos.x][pos.y] = std::move(tile);
}

TileVector2D glimmer::Chunk::GetPosition() const {
    return position;
}

glimmer::Tile *glimmer::Chunk::GetTile(const TileLayerType layerType, const int x, const int y) {
    return tiles_[layerType][x][y].get();
}

glimmer::Tile *glimmer::Chunk::GetTile(const TileLayerType layerType, const TileVector2D &tileVector2d) {
    return GetTile(layerType, tileVector2d.x, tileVector2d.y);
}

std::unique_ptr<glimmer::Tile> glimmer::Chunk::ReplaceTile(const TileLayerType layerType,
                                                           const TileVector2D &tileVector2d,
                                                           std::unique_ptr<Tile> newTile) {
    std::unique_ptr<Tile> extracted = std::move(tiles_[layerType][tileVector2d.x][tileVector2d.y]);
    tiles_[layerType][tileVector2d.x][tileVector2d.y] = std::move(newTile);
    return extracted;
}
