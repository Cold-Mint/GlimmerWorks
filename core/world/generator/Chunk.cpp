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

#include <utility>

#include "core/ecs/EntityManager.h"
#include "core/ecs/component/ItemContainerComponent.h"
#include "core/inventory/ItemContainer.h"
#include "core/log/LogCat.h"
#include "core/math/CoordinateTransformer.h"
#include "core/utils/RandomUtils.h"
#include "core/world/TileInstancePool.h"
#include "core/world/WorldContext.h"
#include "src/saves/tile_state.pb.h"

bool glimmer::Chunk::RemoveReplaceTileCallback(const long index) {
    if (replaceTileCallback_.size() <= index) {
        return false;
    }
    replaceTileCallback_.erase(replaceTileCallback_.begin() + index);
    return true;
}


bool glimmer::Chunk::CommitTileState(const BreakSource breakSource, const TileLayerType layerType, size_t index,
                                     const bool fallback) {
    if (index >= CHUNK_AREA) {
        return false;
    }
    const auto relative = CoordinateTransformer::IndexToVector2D<ChunkRelativeVector2D>(index,
        CHUNK_MASK,
        CHUNK_SHIFT);
    TileVector2D worldTileVector2D;
    worldTileVector2D.x = static_cast<int>(relative.x) + position_.x;
    worldTileVector2D.y = static_cast<int>(relative.y) + position_.y;
    const auto tileStateIterator = tileState_.find(layerType);
    if (tileStateIterator == tileState_.end()) {
        return false;
    }
    const std::shared_ptr<TileStateMessage> &tileStateMessageShared = tileStateIterator->second[index];
    const TileStateMessage *tileStateMessage = tileStateMessageShared.get();
    if (tileStateMessage == nullptr) {
        return false;
    }
    //Check if it is necessary to rebuild the tiles.
    //检查是否需要重建瓦片。
    ResourceRef resourceRef;
    resourceRef.ReadResourceRefMessage(tileStateMessage->resourceref());
    const uint64_t fingerprint = resourceRef.GetFingerprint();
    bool rebuildTile = false;
    if (const auto iterator = tileFingerprint_.find(layerType); iterator == tileFingerprint_.end()) {
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
            LogCat::w(LogLabel::CHUNK, std::source_location::current(), "chunk_tile_resource_null",
                      "Tile resource is null, cannot rebuild tile: layer={}, index={}",
                      std::to_underlying(layerType), index);
            return false;
        }
        TileInstancePool *tileInstancePool = worldContext_->GetTileInstancePool();
        if (tileInstancePool == nullptr) {
            return false;
        }

        const std::shared_ptr<Tile> newTile = tileInstancePool->CreateTile(appContext, tileResource,
                                                                           fingerprint);
        if (newTile == nullptr) {
            LogCat::w(LogLabel::CHUNK, std::source_location::current(), "chunk_create_tile_failed",
                      "Failed to create tile instance: layer={}, index={}", std::to_underlying(layerType),
                      index);
            return false;
        }
        std::shared_ptr<Tile> oldTile = nullptr;
        auto [tileIterator, tileInserted] = tiles_.try_emplace(layerType);
        oldTile = tileIterator->second[index];
        if (oldTile != nullptr) {
            oldTile->OnBreak(worldContext_, breakSource, worldTileVector2D);
        }
        tileIterator->second[index] = newTile;
        newTile->OnPlace(worldContext_, tileStateMessage->placesource(), worldTileVector2D, tileStateMessage);
        auto [tileFingerprintIterator, tileFingerprintInserted] = tileFingerprint_.try_emplace(layerType);
        tileFingerprintIterator->second[index] = fingerprint;
        auto [tileSnapshotIterator,tileSnapshotInserted] = tileSnapshots_.try_emplace(layerType);
        std::shared_ptr<TileSnapshot> &tileSnapshotPtr = tileSnapshotIterator->second[index];
        if (tileSnapshotPtr == nullptr) {
            tileSnapshotPtr = std::make_shared<TileSnapshot>();
        }
        tileSnapshotPtr->SetTile(newTile);
        tileSnapshotPtr->SetTileState(tileStateMessageShared);
        InvokeReplaceTileCallback(shared_from_this(), layerType, index, oldTile, newTile, breakSource);
    }
    return true;
}

TileStateMessage *glimmer::Chunk::GetTileState(const TileLayerType layerType, const size_t index) const {
    const auto it = tileState_.find(layerType);
    if (it == tileState_.end()) {
        return nullptr;
    }
    return it->second[index].get();
}


TileStateMessage *glimmer::Chunk::GetOrCreateTileState(const TileLayerType layerType, const size_t index) {
    TileStateMessage *tileStateMessage = GetTileState(layerType, index);
    if (tileStateMessage == nullptr) {
        auto tileState = std::make_shared<TileStateMessage>();
        auto [tileStateIterator, tileStateInserted] = tileState_.try_emplace(layerType);
        tileStateIterator->second[index] = std::move(tileState);
        return tileStateIterator->second[index].get();
    }
    return tileStateMessage;
}

void glimmer::Chunk::InitGrowthState(TileStateMessage *msg, const TileResource *tileResource, uint64_t tick) {
    if (msg == nullptr || tileResource == nullptr) {
        return;
    }
    msg->set_tileplacedtick(tick);
    msg->set_tilegrowthstarttick(tick);
    msg->set_tilegrowthaccumulatedtick(0);
    msg->set_maturecount(0);
    msg->set_tilegrowthconditionsmet(false);
    if (Tile::IsCropsBlock(tileResource->growthMinTicks, tileResource->growthTarget)) {
        if (tileResource->growthMinTicks == tileResource->growthMaxTicks) {
            msg->set_tilegrowthrequiredtick(tileResource->growthMinTicks);
        } else {
            msg->set_tilegrowthrequiredtick(
                RandomUtils::Random(tileResource->growthMinTicks, tileResource->growthMaxTicks));
        }
    } else {
        msg->set_tilegrowthrequiredtick(0);
    }
}

const glimmer::ChunkVertexVector2D &glimmer::Chunk::GetPosition() const {
    return position_;
}


bool glimmer::Chunk::PlaceTileCell(const TileLayerType layerType, const size_t index,
                                   const ResourceRef &resourceRef, const TileResource *tileResource,
                                   const BreakSource breakSource, const PlaceSourceMessage placeSource,
                                   const int cellX, const int cellY, const bool fallback) {
    if (tileResource == nullptr) {
        return false;
    }
    TileStateMessage *tileStateMessage = GetOrCreateTileState(layerType, index);
    if (tileStateMessage == nullptr) {
        return false;
    }
    tileStateMessage->set_width(tileResource->tileWidth);
    tileStateMessage->set_height(tileResource->tileHeight);
    tileStateMessage->set_placesource(placeSource);
    //The renderer reconstructs the tile's top-left origin via "tileCoord + offset", so the stored
    //offset must be "top-left - cell". cellX/cellY are measured from the top-left origin: cellX is the
    //column to the right and cellY is the row below the top-left cell.
    //渲染器通过 "tileCoord + offset" 还原瓦片左上原点，故存储的 offset 必须是「左上角 - 当前格」。
    //cellX/cellY 以左上为原点度量：cellX 为向右的列，cellY 为向下的行。
    tileStateMessage->mutable_offset()->set_x(-cellX);
    tileStateMessage->mutable_offset()->set_y(cellY);
    resourceRef.WriteResourceRefMessage(*tileStateMessage->mutable_resourceref());
    const uint64_t tick = worldContext_ != nullptr ? worldContext_->GetGlobalTick() : 0;
    InitGrowthState(tileStateMessage, tileResource, tick);
    return CommitTileState(breakSource, layerType, index, fallback);
}

void glimmer::Chunk::PlaceTileAt(const TileLayerType layerType, const size_t topLeftIndex,
                                 const ResourceRef &resourceRef, const TileResource *tileResource,
                                 const BreakSource breakSource, const PlaceSourceMessage placeSource,
                                 const bool fallback) {
    if (tileResource == nullptr) {
        return;
    }
    for (int x = 0; x < tileResource->tileWidth; x++) {
        for (int y = 0; y < tileResource->tileHeight; y++) {
            //Cells extend right (+x) and down (+y); each row down decreases the chunk-local index by CHUNK_SIZE.
            //格子向右(+x)向下(+y)延伸；每向下一行，区块内索引减少 CHUNK_SIZE。
            const size_t index = topLeftIndex + x - static_cast<size_t>(y) * CHUNK_SIZE;
            PlaceTileCell(layerType, index, resourceRef, tileResource, breakSource, placeSource, x, y, fallback);
        }
    }
}


const glimmer::Tile *glimmer::Chunk::GetTile(const TileLayerType layerType, const size_t index) const {
    const auto it = tiles_.find(layerType);
    if (it == tiles_.end()) {
        return nullptr;
    }
    return it->second[index].get();
}

std::shared_ptr<glimmer::Tile> glimmer::Chunk::GetTileShared(const TileLayerType layerType, const size_t index) const {
    const auto it = tiles_.find(layerType);
    if (it == tiles_.end()) {
        return nullptr;
    }
    return it->second[index];
}

std::vector<std::shared_ptr<glimmer::TileSnapshot> > glimmer::Chunk::GetTopVisibleTileSnapshots(
    const std::byte layerFilter,
    const size_t index) const {
    std::vector<std::shared_ptr<TileSnapshot> > topVisibleTileSnapshots;
    if (index >= CHUNK_AREA) {
        return topVisibleTileSnapshots;
    }
    for (int i = 0; i < TILE_LAYER_TYPE_COUNT; i++) {
        auto layer = static_cast<std::byte>(1 << i);
        if ((layerFilter & layer) != std::byte{0}) {
            auto tileLayerType = static_cast<TileLayerType>(layer);
            auto iterator = tileSnapshots_.find(tileLayerType);
            if (iterator == tileSnapshots_.end()) {
                continue;
            }

            const std::array<std::shared_ptr<TileSnapshot>, CHUNK_AREA> &tileArray = iterator->second;
            const std::shared_ptr<TileSnapshot> &tileSnapshot = tileArray[index];
            if (tileSnapshot == nullptr) {
                continue;
            }
            topVisibleTileSnapshots.emplace_back(tileSnapshot);
        }
    }
    return topVisibleTileSnapshots;
}


void glimmer::Chunk::ReadChunkMessage(const ChunkMessage &chunkMessage) {
    position_.ReadVector2DIMessage(chunkMessage.position());
    tiles_.clear();
    tileState_.clear();
    LogCat::i(LogLabel::CHUNK, "chunk_read_message", "Reading chunk message: position=({}, {}), layer count={}",
              position_.x,
              position_.y, chunkMessage.tilestates().size());
    for (auto &map = chunkMessage.tilestates(); const auto &[layerTypeNumber, tileData]: map) {
        const auto layerType = static_cast<TileLayerType>(layerTypeNumber);
        const auto tileResourceRefSize = tileData.tilestatemessage_size();
        for (int i = 0; i < tileResourceRefSize; i++) {
            auto &tileStateMessage = tileData.tilestatemessage(i);
            const auto tileStatePtr = GetOrCreateTileState(layerType, i);
            tileStatePtr->CopyFrom(tileStateMessage);
            CommitTileState(BreakSource::ChunkLoad, layerType, i, true);
        }
    }
}


void glimmer::Chunk::ApplyOfflineGrowth() {
    if (worldContext_ == nullptr) {
        return;
    }
    const uint64_t globalTick = worldContext_->GetGlobalTick();
    size_t advancedCount = 0;
    for (auto &[layerType, tileArray]: tiles_) {
        for (size_t index = 0; index < CHUNK_AREA; ++index) {
            const std::shared_ptr<Tile> &tile = tileArray[index];
            if (tile == nullptr || !tile->IsCropsBlock() || !tile->AllowOfflineGrowth()) {
                continue;
            }
            TileStateMessage *state = GetTileState(layerType, index);
            if (state == nullptr) {
                continue;
            }
            //Only advance crops whose conditions were met when the chunk was last saved.
            //只推进上次保存时条件已满足的作物。
            if (!state->tilegrowthconditionsmet()) {
                continue;
            }
            if (globalTick <= state->tilegrowthstarttick()) {
                continue;
            }
            const uint64_t elapsed = globalTick - state->tilegrowthstarttick();
            state->set_tilegrowthaccumulatedtick(state->tilegrowthaccumulatedtick() + elapsed);
            state->set_tilegrowthstarttick(globalTick);
            ++advancedCount;
        }
    }
    if (advancedCount > 0) {
        LogCat::i(LogLabel::CHUNK, "chunk_offline_growth_applied",
                  "Applied offline growth to {} crop tiles at chunk ({}, {})",
                  advancedCount, position_.x, position_.y);
    }
}

void glimmer::Chunk::BreakTileEntities() {
    if (worldContext_ == nullptr) {
        return;
    }
    for (auto &tileArray: tiles_ | std::views::values) {
        for (size_t index = 0; index < CHUNK_AREA; ++index) {
            const std::shared_ptr<Tile> &tile = tileArray[index];
            if (tile == nullptr) {
                continue;
            }
            const auto relative = CoordinateTransformer::IndexToVector2D<ChunkRelativeVector2D>(index,
                CHUNK_MASK,
                CHUNK_SHIFT);
            const TileVector2D worldTile{
                static_cast<int>(relative.x) + position_.x,
                static_cast<int>(relative.y) + position_.y
            };
            tile->OnBreak(worldContext_, BreakSource::ChunkUnload, worldTile);
        }
    }
}

void glimmer::Chunk::InvokeReplaceTileCallback(const std::shared_ptr<Chunk> &chunk, const TileLayerType layerType,
                                               const size_t index,
                                               const std::shared_ptr<Tile> &oldTile,
                                               const std::shared_ptr<Tile> &newTile,
                                               const BreakSource breakSource) const {
    for (auto &replaceTileCallback: replaceTileCallback_) {
        replaceTileCallback(chunk, layerType, index, oldTile, newTile, breakSource);
    }
}

void glimmer::Chunk::WriteTileStatesToMessage(
    const std::array<std::shared_ptr<TileStateMessage>, CHUNK_AREA> &tileStates,
    TileStateArrayMessage &layerMessage) {
    layerMessage.mutable_tilestatemessage()->Reserve(CHUNK_AREA);
    for (int y = 0; y < CHUNK_SIZE; y++) {
        for (int x = 0; x < CHUNK_SIZE; x++) {
            const size_t index = CoordinateTransformer::GetArrayIndex(x, y, CHUNK_SHIFT);
            const TileStateMessage *tileStateMessage = tileStates[index].get();
            auto *modifiableTileStateMessage = layerMessage.add_tilestatemessage();
            if (tileStateMessage == nullptr) {
                continue;
            }
            modifiableTileStateMessage->CopyFrom(*tileStateMessage);
        }
    }
}

glimmer::Chunk::Chunk(WorldContext *worldContext, ChunkVertexVector2D pos) : position_(std::move(pos)),
                                                                             worldContext_(worldContext) {
}

void glimmer::Chunk::SetDependencyTerrain(std::shared_ptr<TerrainResult> dependencyTerrain) {
    dependencyTerrain_ = std::move(dependencyTerrain);
}

size_t glimmer::Chunk::AddReplaceTileCallback(
    const std::function<void(std::shared_ptr<Chunk> chunk, TileLayerType layerType, size_t index, std::shared_ptr<Tile>
                             oldTile, std::shared_ptr<Tile> newTile, BreakSource breakSource)> &callBack) {
    replaceTileCallback_.emplace_back(callBack);
    return replaceTileCallback_.size() - 1;
}

void glimmer::Chunk::WriteChunkMessage(ChunkMessage &chunkMessage) {
    position_.WriteVector2DIMessage(*chunkMessage.mutable_position());
    chunkMessage.clear_tilestates();
    LogCat::d(LogLabel::CHUNK, "chunk_write_message", "Writing chunk message: position=({}, {})", position_.x,
              position_.y);
    //Sync the live storage container of each storage block into its tile state before saving.
    //存档前把每个存储方块的活动容器同步进其瓦片状态。
    if (worldContext_ != nullptr) {
        if (EntityManager *entityManager = worldContext_->GetEntityManager(); entityManager != nullptr) {
            for (const auto &[layerType, tileArray]: tiles_) {
                const auto tileStateIt = tileState_.find(layerType);
                if (tileStateIt == tileState_.end()) {
                    continue;
                }
                for (size_t index = 0; index < CHUNK_AREA; ++index) {
                    const std::shared_ptr<Tile> &tile = tileArray[index];
                    if (tile == nullptr || !tile->IsStorageBlock()) {
                        continue;
                    }
                    const std::shared_ptr<TileStateMessage> &tileStatePtr = tileStateIt->second[index];
                    if (tileStatePtr == nullptr) {
                        continue;
                    }
                    const auto relative = CoordinateTransformer::IndexToVector2D<ChunkRelativeVector2D>(index,
                        CHUNK_MASK, CHUNK_SHIFT);
                    const GameEntityID entityId = tile->GetGameEntity(
                        CoordinateTransformer::ChunkRelativeToTile(position_, relative));
                    if (WorldContext::IsEmptyEntityId(entityId)) {
                        continue;
                    }
                    const auto *containerComponent = entityManager->GetComponent<ItemContainerComponent>(entityId);
                    if (containerComponent == nullptr) {
                        continue;
                    }
                    const ItemContainer *container = containerComponent->GetItemContainer();
                    if (container == nullptr) {
                        continue;
                    }
                    container->ToMessage(*tileStatePtr->mutable_itemcontainer());
                }
            }
        }
    }
    for (const auto &[layerType, tileArray]: tileState_) {
        auto &tileData =
                (*chunkMessage.mutable_tilestates())[std::to_underlying(layerType)];
        WriteTileStatesToMessage(tileArray, tileData);
    }
}

glimmer::WorldVector2D glimmer::Chunk::GetStartWorldPosition() const {
    return CoordinateTransformer::TileToWorld(CoordinateTransformer::ChunkVertexToTile(position_));
}

glimmer::WorldVector2D glimmer::Chunk::GetEndWorldPosition() const {
    return CoordinateTransformer::TileToWorld(
        CoordinateTransformer::ChunkVertexToTile(position_) + TileVector2D(CHUNK_SIZE, CHUNK_SIZE));
}
