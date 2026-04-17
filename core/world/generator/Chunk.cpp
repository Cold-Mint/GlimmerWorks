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

size_t glimmer::Chunk::AddSetTileCallback(
    const std::function<void(Chunk *chunk, int index, Tile *tile, TileLayerType layerType)> &callBack) {
    setTileCallback_.emplace_back(callBack);
    return setTileCallback_.size() - 1;
}

bool glimmer::Chunk::RemoveSetTileCallback(const long index) {
    if (setTileCallback_.size() <= index) {
        return false;
    }
    setTileCallback_.erase(setTileCallback_.begin() + index);
    return true;
}

size_t glimmer::Chunk::AddReplaceTileCallback(
    const std::function<void(Chunk *chunk, TileLayerType layerType, int index, Tile *oldTile,
                             Tile *
                             newTile)> &callBack) {
    replaceTileCallback_.emplace_back(callBack);
    return replaceTileCallback_.size() - 1;
}

bool glimmer::Chunk::RemoveReplaceTileCallback(const long index) {
    if (replaceTileCallback_.size() <= index) {
        return false;
    }
    replaceTileCallback_.erase(replaceTileCallback_.begin() + index);
    return true;
}

TileVector2D glimmer::Chunk::TileCoordinatesToChunkVertexCoordinates(const TileVector2D tileVector2d) {
    return {
        tileVector2d.x & CHUNK_ALIGN,
        tileVector2d.y & CHUNK_ALIGN
    };
}


TileVector2D glimmer::Chunk::TileCoordinatesToChunkRelativeCoordinates(const TileVector2D tileVector2d) {
    return {
        tileVector2d.x & CHUNK_MASK,
        tileVector2d.y & CHUNK_MASK
    };
}

void glimmer::Chunk::InvokeSetTileCallback(Chunk *chunk, const int index, Tile *tile,
                                           const TileLayerType layerType) const {
    for (auto &tileCallback: setTileCallback_) {
        tileCallback(chunk, index, tile, layerType);
    }
}

void glimmer::Chunk::InvokeReplaceTileCallback(Chunk *chunk, TileLayerType layerType, int index, Tile *oldTile,
                                               Tile *newTile) const {
    for (auto &replaceTileCallback: replaceTileCallback_) {
        replaceTileCallback(chunk, layerType, index, oldTile, newTile);
    }
}


glimmer::Chunk::Chunk(const TileVector2D &pos) : position(pos) {
}

void glimmer::Chunk::SetTile(const TileVector2D pos, std::unique_ptr<Tile> tile) {
    if (tile == nullptr) {
        return;
    }
    const TileLayerType tileLayer = tile->GetLayerType();
    SetTileToLayer(pos.y << CHUNK_SHIFT | pos.x, tileLayer, std::move(tile));
}

void glimmer::Chunk::SetTileToLayer(const TileVector2D pos, const TileLayerType layerType, std::unique_ptr<Tile> tile) {
    SetTileToLayer(pos.y << CHUNK_SHIFT | pos.x, layerType, std::move(tile));
}


void glimmer::Chunk::SetTileToLayer(const int index, const TileLayerType layerType, std::unique_ptr<Tile> tile) {
    if (tile == nullptr || index < 0 || index >= CHUNK_AREA) {
        return;
    }
    Tile *tilePtr = tile.get();
    TileLayerType targetLayerType = tile->GetLayerType();
    if (tilePtr->ChangeLayerTypeIfAllowed(layerType)) {
        targetLayerType = tile->GetLayerType();
    }
    auto [it, inserted] = tiles_.try_emplace(targetLayerType);

    it->second[index] = std::move(tile);
    InvokeSetTileCallback(this, index, tilePtr, targetLayerType);
}

TileVector2D glimmer::Chunk::GetPosition() const {
    return position;
}

glimmer::Tile *glimmer::Chunk::GetTile(TileLayerType layerType, int x, int y) const {
    return GetTile(layerType, y << CHUNK_SHIFT | x);
}

glimmer::Tile *glimmer::Chunk::GetTile(TileLayerType layerType, int index) const {
    const auto it = tiles_.find(layerType);
    if (it == tiles_.end()) {
        return nullptr;
    }
    if (index < 0 || index >= CHUNK_AREA) {
        return nullptr;
    }
    return it->second[index].get();
}

glimmer::Tile *glimmer::Chunk::GetTile(const TileLayerType layerType, const TileVector2D &tileVector2d) const {
    return GetTile(layerType, tileVector2d.y << CHUNK_SHIFT | tileVector2d.x);
}

std::vector<glimmer::Tile *> glimmer::Chunk::GetTopVisibleTiles(const uint8_t layerFilter,
                                                                const TileVector2D &tileVector2d) const {
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
            SetTileToLayer(i, layerType, std::move(tile));
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
    if (newTile == nullptr) {
        return nullptr;
    }
    const int index = tileVector2d.y << CHUNK_SHIFT | tileVector2d.x;
    const auto it = tiles_.find(layerType);
    if (it == tiles_.end() || index < 0 || index >= CHUNK_AREA) {
        return nullptr;
    }
    std::unique_ptr<Tile> extracted = std::move(it->second[index]);
    Tile *newTilePtr = newTile.get();
    it->second[index] = std::move(newTile);
    InvokeReplaceTileCallback(this, layerType, index, extracted.get(), newTilePtr);
    return extracted;
}
