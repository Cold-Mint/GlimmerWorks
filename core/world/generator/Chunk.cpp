/*
 * Copyright (C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * 版权(C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * 本程序是自由软件：你可以遵照自由软件基金会出版的GNU Affero通用公共许可证条款来重新分发和修改它
 * 该许可证的第3版，或者（由你选择）任何后续版本。
 *
 * 本程序的发布目的是希望它能有用，但没有任何担保；甚至没有适销性或特定用途适用性的默示担保。
 * 有关详细细节，请参阅GNU Affero通用公共许可证。
 *
 * 你应该已经收到一份GNU Affero通用公共许可证的副本。如果没有，请查阅<https://www.gnu.org/licenses/>。
 */
#include "Chunk.h"

#include "core/math/CoordinateTransformer.h"
#include "core/world/WorldContext.h"
#include "src/saves/tile_state.pb.h"

void glimmer::Chunk::AddBodyId(b2BodyId bodyId) { attachedBodies_.emplace_back(bodyId); }

float glimmer::Chunk::GetChunkFadeAlpha() const {
    return chunkFadeAlpha_;
}

const std::vector<b2BodyId> &glimmer::Chunk::GetAttachedBodies() {
    return attachedBodies_;
}

void glimmer::Chunk::ClearAttachedBodies() {
    attachedBodies_.clear();
}

size_t glimmer::Chunk::AddReplaceTileCallback(
    const std::function<void(Chunk *chunk, TileLayerType layerType, int index, std::shared_ptr<Tile> oldTile, std::
                             shared_ptr<Tile> newTile)> &callBack) {
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

glimmer::TileVector2D glimmer::Chunk::TileCoordinatesToChunkVertexCoordinates(const TileVector2D tileVector2d) {
    return {
        tileVector2d.x & CHUNK_ALIGN,
        tileVector2d.y & CHUNK_ALIGN
    };
}


glimmer::TileVector2D glimmer::Chunk::TileCoordinatesToChunkRelativeCoordinates(const TileVector2D tileVector2d) {
    return {
        tileVector2d.x & CHUNK_MASK,
        tileVector2d.y & CHUNK_MASK
    };
}

bool glimmer::Chunk::CommitTileState(const TileLayerType layerType, const int index, const bool fallback) {
    if (index < 0 || index >= CHUNK_AREA) {
        return false;
    }
    const TileStateMessage *tileStateMessage = GetTileState(layerType, index);
    if (tileStateMessage == nullptr) {
        return false;
    }
    //Check if it is necessary to rebuild the tiles.
    //检查是否需要重建瓦片。
    ResourceRef resourceRef;
    resourceRef.ReadResourceRefMessage(tileStateMessage->resourceref());
    const uint64_t fingerprint = resourceRef.GetFingerprint();
    bool rebuildTile = false;
    auto iterator = tileFingerprint_.find(layerType);
    if (iterator == tileFingerprint_.end()) {
        rebuildTile = true;
    } else {
        rebuildTile = iterator->second[index] != fingerprint;
    }
    if (rebuildTile) {
        if (worldContext_ == nullptr) {
            return false;
        }
        const AppContext *appContext = worldContext_->GetAppContext();
        if (appContext == nullptr) {
            return false;
        }

        const ResourceLocator *resourceLocator = appContext->GetResourceLocator();
        if (resourceLocator == nullptr) {
            return false;
        }
        const TileResource *tileResource = nullptr;
        if (fallback) {
            tileResource = resourceLocator->FindTileFallback(&resourceRef, layerType);
        } else {
            tileResource = resourceLocator->FindTileRaw(&resourceRef);
        }
        if (tileResource == nullptr) {
            return false;
        }
        TileInstancePool *tileInstancePool = worldContext_->GetTileInstancePool();
        if (tileInstancePool == nullptr) {
            return false;
        }

        const std::shared_ptr<Tile> newTile = tileInstancePool->CreateTile(appContext, tileResource,
                                                                           fingerprint);
        if (newTile == nullptr) {
            return false;
        }
        std::shared_ptr<Tile> oldTile = nullptr;
        auto [tileIterator, tileInserted] = tiles_.try_emplace(layerType);
        oldTile = tileIterator->second[index];
        tileIterator->second[index] = newTile;
        auto [tileFingerprintIterator, tileFingerprintInserted] = tileFingerprint_.try_emplace(layerType);
        tileFingerprintIterator->second[index] = fingerprint;
        InvokeReplaceTileCallback(this, layerType, index, oldTile, newTile);
    }
    return true;
}

TileStateMessage *glimmer::Chunk::GetTileState(const TileLayerType layerType, const uint8_t index) const {
    const auto it = tileState_.find(layerType);
    if (it == tileState_.end()) {
        return nullptr;
    }
    return it->second[index].get();
}


void glimmer::Chunk::InvokeReplaceTileCallback(Chunk *chunk, const TileLayerType layerType, const int index,
                                               const std::shared_ptr<Tile> &oldTile,
                                               const std::shared_ptr<Tile> &newTile) const {
    for (auto &replaceTileCallback: replaceTileCallback_) {
        replaceTileCallback(chunk, layerType, index, oldTile, newTile);
    }
}

glimmer::Chunk::Chunk(WorldContext *worldContext, const TileVector2D &pos,
                      const AnimConfig &animConfig) : position(pos) {
    worldContext_ = worldContext;
    chunkFadeAlpha_ = animConfig.chunkFadeInFrom;
    chunkFadeInTween_ = tweeny::tween<float>::from(animConfig.chunkFadeInFrom).to(animConfig.chunkFadeInTo).during(
        animConfig.chunkFadeinDuration * 1000).via(tweeny::easing::cubicOut);
}


void glimmer::Chunk::UpdateFadeInAnimation(const float delta) {
    if (!chunkFadeInTween_.isFinished()) {
        chunkFadeInTween_.step(delta);
        chunkFadeAlpha_ = chunkFadeInTween_.peek();
    }
}

TileStateMessage *glimmer::Chunk::GetOrCreateTileState(const TileLayerType layerType, const int index) {
    TileStateMessage *tileStateMessage = GetTileState(layerType, index);
    if (tileStateMessage == nullptr) {
        auto tileState = std::make_unique<TileStateMessage>();
        auto [tileStateIterator, tileStateInserted] = tileState_.try_emplace(layerType);
        tileStateIterator->second[index] = std::move(tileState);
        return tileStateIterator->second[index].get();
    }
    return tileStateMessage;
}

glimmer::TileVector2D glimmer::Chunk::GetPosition() const {
    return position;
}

const glimmer::Tile *glimmer::Chunk::GetTile(const TileLayerType layerType, const uint8_t index) const {
    const auto it = tiles_.find(layerType);
    if (it == tiles_.end()) {
        return nullptr;
    }
    return it->second[index].get();
}

std::shared_ptr<glimmer::Tile> glimmer::Chunk::GetTileShared(TileLayerType layerType, uint8_t index) const {
    const auto it = tiles_.find(layerType);
    if (it == tiles_.end()) {
        return nullptr;
    }
    return it->second[index];
}

std::unique_ptr<std::vector<glimmer::TileSnapshot> > glimmer::Chunk::GetTopVisibleTileSnapshots(
    const uint8_t layerFilter,
    const uint8_t index) const {
    auto tileSnapshotVector = std::make_unique<std::vector<TileSnapshot> >();
    for (int i = 0; i < TILE_LAYER_TYPE_COUNT; i++) {
        const uint8_t layer = 1 << i;
        if (layerFilter && layer) {
            //If the i-th position is 1.
            //如果第i位为1。
            const auto layerType = static_cast<TileLayerType>(layer);
            const Tile *tile = GetTile(layerType, index);
            if (tile == nullptr) {
                continue;
            }
            const TileStateMessage *tileStateMessage = GetTileState(layerType, index);
            if (tileStateMessage == nullptr) {
                continue;
            }
            TileSnapshot tileSnapshot;
            tileSnapshot.SetTile(tile);
            tileSnapshot.SetTileState(tileStateMessage);
            tileSnapshotVector->emplace_back(tileSnapshot);
        }
    }
    return tileSnapshotVector;
}

void glimmer::Chunk::ReadChunkMessage(const ChunkMessage &chunkMessage) {
    position.ReadVector2DIMessage(chunkMessage.position());
    tiles_.clear();
    tileState_.clear();
    auto &map = chunkMessage.tilestates();
    for (const auto &mapPair: map) {
        const auto layerType = static_cast<TileLayerType>(mapPair.first);
        const TileStateArrayMessage &tileData = mapPair.second;
        auto tileResourceRefSize = tileData.tilestatemessage_size();
        std::array<std::unique_ptr<Tile>, CHUNK_AREA> value;
        for (int i = 0; i < tileResourceRefSize; i++) {
            auto &tileStateMessage = tileData.tilestatemessage(i);
            auto tileStatePtr = GetOrCreateTileState(layerType, i);
            tileStatePtr->CopyFrom(tileStateMessage);
            CommitTileState(layerType, i, true);
        }
    }
}


void glimmer::Chunk::WriteChunkMessage(ChunkMessage &chunkMessage) {
    position.WriteVector2DIMessage(*chunkMessage.mutable_position());
    chunkMessage.clear_tilestates();
    for (const auto &[layerType, tileArray]: tileState_) {
        auto &tileData =
                (*chunkMessage.mutable_tilestates())[static_cast<uint8_t>(layerType)];
        tileData.mutable_tilestatemessage()->Reserve(
            CHUNK_AREA
        );
        for (int y = 0; y < CHUNK_SIZE; y++) {
            for (int x = 0; x < CHUNK_SIZE; x++) {
                const auto modifiableTileStateMessage = tileData.add_tilestatemessage();
                const int index = y << CHUNK_SHIFT | x;
                const TileStateMessage *tileStateMessage = tileArray[index].get();
                if (tileStateMessage == nullptr) {
                    continue;
                }
                modifiableTileStateMessage->CopyFrom(*tileStateMessage);
            }
        }
    }
}

glimmer::WorldVector2D glimmer::Chunk::GetStartWorldPosition() const {
    return CoordinateTransformer::TileToWorld(position);
}

glimmer::WorldVector2D glimmer::Chunk::GetEndWorldPosition() const {
    return CoordinateTransformer::TileToWorld(position + TileVector2D(CHUNK_SIZE, CHUNK_SIZE));
}
