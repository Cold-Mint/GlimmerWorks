//
// Created by Cold-Mint on 2025/11/4.
//

#include "Chunk.h"

#include "../log/LogCat.h"
#include "../mod/ResourceLocator.h"
#include "../scene/AppContext.h"


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

glimmer::Chunk::Chunk(const TileVector2D &pos) : position(pos) {
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

void glimmer::Chunk::FromMessage(AppContext *appContext, const ChunkMessage &chunkMessage) {
    position.FromMessage(chunkMessage.position());
    tiles_.clear();
    auto map = chunkMessage.tiles();
    for (auto mapPair: map) {
        auto key = static_cast<TileLayerType>(mapPair.first);
        auto tileData = mapPair.second;
        auto tileResourceRefSize = tileData.tileresourceref_size();
        std::array<std::array<std::unique_ptr<Tile>, CHUNK_SIZE>, CHUNK_SIZE> value;
        for (int i = 0; i < tileResourceRefSize; i++) {
            auto tileResourceRefMessage = tileData.tileresourceref(i);
            int x = i % CHUNK_SIZE;
            int y = i / CHUNK_SIZE;
            ResourceRef resourceRef;
            resourceRef.FromMessage(tileResourceRefMessage);
            auto tileResource = appContext->GetResourceLocator()->FindTile(resourceRef);
            if (!tileResource.has_value()) {
                continue;
            }
            value[x][y] = Tile::FromResourceRef(appContext, tileResource.value());
        }
        tiles_.emplace(key, std::move(value));
    }
}

void glimmer::Chunk::ToMessage(ChunkMessage &chunkMessage) {
    position.ToMessage(*chunkMessage.mutable_position());
    chunkMessage.clear_tiles();

    for (const auto &[layerType, tileArray]: tiles_) {
        auto &tileData =
                (*chunkMessage.mutable_tiles())[static_cast<int>(layerType)];
        tileData.mutable_tileresourceref()->Reserve(
            CHUNK_SIZE * CHUNK_SIZE
        );
        for (int y = 0; y < CHUNK_SIZE; y++) {
            for (int x = 0; x < CHUNK_SIZE; x++) {
                const auto refMessage = tileData.add_tileresourceref();
                const Tile *tile = tileArray[x][y].get();
                std::optional<ResourceRef> resourceRef = Resource::ParseFromId(tile->id, RESOURCE_TYPE_TILE);
                if (!resourceRef.has_value()) {
                    continue;
                }
                resourceRef->ToMessage(*refMessage);
            }
        }
    }
}

WorldVector2D glimmer::Chunk::GetStartWorldPosition() const {
    return TileLayerComponent::TileToWorld(position);
}

WorldVector2D glimmer::Chunk::GetEndWorldPosition() const {
    return TileLayerComponent::TileToWorld(position + TileVector2D(CHUNK_SIZE, CHUNK_SIZE));
}

std::unique_ptr<glimmer::Tile> glimmer::Chunk::ReplaceTile(const TileLayerType layerType,
                                                           const TileVector2D &tileVector2d,
                                                           std::unique_ptr<Tile> newTile) {
    std::unique_ptr<Tile> extracted = std::move(tiles_[layerType][tileVector2d.x][tileVector2d.y]);
    tiles_[layerType][tileVector2d.x][tileVector2d.y] = std::move(newTile);
    return extracted;
}
