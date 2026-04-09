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
        tileVector2d.x & CHUNK_ALIGN,
        tileVector2d.y & CHUNK_ALIGN
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
    return {
        tileVector2d.x & CHUNK_MASK,
        tileVector2d.y & CHUNK_MASK
    };
}

glimmer::Chunk::Chunk(const TileVector2D &pos) : position(pos) {
}

void glimmer::Chunk::SetTile(const TileVector2D pos, std::unique_ptr<Tile> tile) {
    const int index = pos.y << CHUNK_SHIFT | pos.x;
    tiles_[tile->GetLayerType()][index] = std::move(tile);
}

void glimmer::Chunk::SetTileToLayer(TileVector2D pos, std::unique_ptr<Tile> tile, TileLayerType layerType) {
    const int index = pos.y << CHUNK_SHIFT | pos.x;
    SetTileToLayer(index, std::move(tile), layerType);
}

void glimmer::Chunk::SetTileToLayer(int index, std::unique_ptr<Tile> tile, TileLayerType layerType) {
    TileLayerType targetLayer = tile->GetLayerType();
    if (tile->SetLayerType(layerType)) {
        targetLayer = layerType;
    }
    tiles_[targetLayer][index] = std::move(tile);
}

TileVector2D glimmer::Chunk::GetPosition() const {
    return position;
}

glimmer::Tile *glimmer::Chunk::GetTile(const TileLayerType layerType, const int x, const int y) {
    return GetTile(layerType, y << CHUNK_SHIFT | x);
}

glimmer::Tile *glimmer::Chunk::GetTile(const TileLayerType layerType, const int index) {
    return tiles_[layerType][index].get();
}

glimmer::Tile *glimmer::Chunk::GetTile(const TileLayerType layerType, const TileVector2D &tileVector2d) {
    return GetTile(layerType, tileVector2d.y << CHUNK_SHIFT | tileVector2d.x);
}

std::vector<glimmer::Tile *> glimmer::Chunk::GetTopVisibleTiles(const uint8_t layerFilter,
                                                                const TileVector2D &tileVector2d) {
    std::vector<Tile *> tiles = {};
    for (int i = 7; i >= 0; i--) {
        const uint8_t layer = 1 << i;
        if (layerFilter && layer) {
            //If the i-th position is 1.
            //如果第i位为1。
            Tile *tile = GetTile(static_cast<TileLayerType>(layer), tileVector2d);
            if (tile == nullptr) {
                continue;
            }
            tiles.emplace_back(tile);
        }
    }
    return tiles;
}

void glimmer::Chunk::ReadChunkMessage(const AppContext *appContext, const ChunkMessage &chunkMessage) {
    position.ReadVector2DIMessage(chunkMessage.position());
    tiles_.clear();
    auto &map = chunkMessage.tiles();
    for (const auto &mapPair: map) {
        auto layerType = static_cast<TileLayerType>(mapPair.first);
        const TileArrayMessage &tileData = mapPair.second;
        auto tileResourceRefSize = tileData.tilemessage_size();
        std::array<std::unique_ptr<Tile>, CHUNK_AREA> value;
        for (int i = 0; i < tileResourceRefSize; i++) {
            auto &tileMessage = tileData.tilemessage(i);
            ResourceRef resourceRef;
            resourceRef.ReadResourceRefMessage(tileMessage.resourceref());
            auto tileResource = appContext->GetResourceLocator()->FindTile(resourceRef);
            if (tileResource == nullptr) {
                continue;
            }
            std::unique_ptr<Tile> tile = Tile::FromTileResource(appContext, tileResource, resourceRef);
            if (tile == nullptr) {
                continue;
            }
            tile->ReadTileMessage(tileMessage);
            SetTileToLayer(i, std::move(tile), layerType);
        }
    }
}

void glimmer::Chunk::WriteChunkMessage(ChunkMessage &chunkMessage) {
    position.WriteVector2DIMessage(*chunkMessage.mutable_position());
    chunkMessage.clear_tiles();
    for (const auto &[layerType, tileArray]: tiles_) {
        auto &tileData =
                (*chunkMessage.mutable_tiles())[static_cast<uint8_t>(layerType)];
        tileData.mutable_tilemessage()->Reserve(
            CHUNK_AREA
        );
        for (int y = 0; y < CHUNK_SIZE; y++) {
            for (int x = 0; x < CHUNK_SIZE; x++) {
                const auto tileMessage = tileData.add_tilemessage();
                const int index = y << CHUNK_SHIFT | x;
                const Tile *tile = tileArray[index].get();
                if (tile == nullptr) {
                    continue;
                }
                tile->WriteTileMessage(*tileMessage);
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
    const int index = tileVector2d.y << CHUNK_SHIFT | tileVector2d.x;
    std::unique_ptr<Tile> extracted = std::move(tiles_[layerType][index]);
    tiles_[layerType][index] = std::move(newTile);
    return extracted;
}
