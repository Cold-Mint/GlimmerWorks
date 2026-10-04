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
#include "ChunkManager.h"

#include <utility>
#include <vector>

#include "core/config/Constants.h"
#include "core/context/AppContext.h"
#include "core/ecs/EntityManager.h"
#include "core/ecs/EntityShortCut.h"
#include "core/ecs/component/Transform2DComponent.h"
#include "core/log/LogCat.h"
#include "core/mod/ResourceLocator.h"
#include "core/saves/Saves.h"
#include "core/scene/MainThreadDispatcher.h"
#include "core/world/Dimension.h"
#include "core/world/Tile.h"
#include "core/world/WorldContext.h"
#include "core/world/structure/StructureGeneratorManager.h"
#include "generator/ChunkGenerator.h"
#include "generator/ChunkLoader.h"
#include "generator/ChunkPhysicsHelper.h"
#include "src/saves/chunk.pb.h"


void glimmer::ChunkManager::UnloadChunkAt(const ResourceRef &dimensionRef, const TileVector2D &position) {
    Chunk *chunk = GetChunk(dimensionRef, position);
    if (chunk == nullptr) {
        return;
    }
    LogCat::d(LogLabel::CHUNK, "chunk_unloading", "Unloading chunk at position: ({}, {})", position.x, position.y);
    if (!SaveChunk(dimensionRef, position)) {
        LogCat::w(LogLabel::CHUNK, std::source_location::current(), "chunk_save_failed_during_unload",
                  "Failed to save chunk during unload at: ({}, {})", position.x,
                  position.y);
        return;
    }
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_SIZE; y++) {
            lightBuffer_->ClearTileLightData(TileVector2D(position.x + x, position.y + y));
        }
    }
    ChunkPhysicsHelper::DetachPhysicsBodyToChunk(chunk);
    const auto dimensionIterator = dimensionMap_.find(dimensionRef);
    if (dimensionIterator == dimensionMap_.end()) {
        return;
    }
    auto &chunkMap = dimensionIterator->second;
    const auto chunkIterator = chunkMap.find(position);
    if (chunkIterator == chunkMap.end()) {
        return;
    }
    chunkMap.erase(chunkIterator);
    LogCat::d(LogLabel::CHUNK, "chunk_unloaded", "Chunk unloaded successfully at: ({}, {})", position.x, position.y);
}

glimmer::Chunk *glimmer::ChunkManager::GetChunk(const ResourceRef &dimensionRef,
                                                const TileVector2D &position) const {
    const auto dimensionIterator = dimensionMap_.find(dimensionRef);
    if (dimensionIterator == dimensionMap_.end()) {
        return nullptr;
    }
    const auto &chunkMap = dimensionIterator->second;
    const auto chunkIterator = chunkMap.find(position);
    if (chunkIterator == chunkMap.end()) {
        return nullptr;
    }
    return chunkIterator->second.get();
}


bool glimmer::ChunkManager::HasChunk(const ResourceRef &dimensionRef, const TileVector2D &position) const {
    return GetChunk(dimensionRef, position) != nullptr;
}


void glimmer::ChunkManager::OnChunkTileChange(Chunk *chunk, [[maybe_unused]] const std::shared_ptr<Tile> &tile,
                                              const TileLayerType layerType, const int index) const {
    if (layerType == TileLayerType::Ground) {
        ChunkPhysicsHelper::UpdatePhysicsBodyToChunk(worldContext_, chunk);
    }
    UpdateTileLight(chunk, layerType, index);
}


void glimmer::ChunkManager::UpdateTileLight(const Chunk *chunk, const TileLayerType layerType, const int index) const {
    const AppContext *appContext = worldContext_->GetAppContext();
    if (appContext == nullptr) {
        return;
    }
    if (chunk == nullptr) {
        return;
    }
    const Tile *tile = chunk->GetTile(layerType, index);
    if (tile == nullptr) {
        return;
    }
    const ResourceLocator *resourceLocator = appContext->GetResourceLocator();
    if (resourceLocator == nullptr) {
        return;
    }
    const TileVector2D chunkPosition = chunk->GetPosition();
    const int localX = index & CHUNK_MASK;
    const int localY = index >> CHUNK_SHIFT;
    auto lightSourcePosition = TileVector2D(chunkPosition.x + localX, chunkPosition.y + localY);
    const TileLightResourceData *tileLightResourceData = tile->GetLightResourceData();
    if (tileLightResourceData == nullptr) {
        return;
    }
    //背光遮照（z 方向）：作用于所有瓦片图层。
    const LightMaskResource *backLightMaskResource = resourceLocator->FindLightMask(
        tileLightResourceData->GetBackLightMaskResource());
    if (backLightMaskResource == nullptr) {
        // Tile has no back light mask resource, clear any existing back light mask data
        // 方块没有背光掩码资源，清除已有的背光掩码数据
        lightBuffer_->ClearLightMask(lightSourcePosition, layerType, LightDirection::Backward);
    } else {
        const std::unique_ptr<Color> backLightMaskColorPtr = resourceLocator->FindColor(
            &backLightMaskResource->lightMaskColor);
        if (backLightMaskColorPtr == nullptr) {
            return;
        }
        if (backLightMaskColorPtr->a == 0) {
            // Resource exists but has zero alpha - clear with re-propagation
            // 资源存在但alpha为0 - 清除并重新传播
            lightBuffer_->ClearLightMask(lightSourcePosition, layerType, LightDirection::Backward);
        } else {
            lightBuffer_->SetLightMask(lightSourcePosition, layerType, LightDirection::Backward,
                                       std::make_unique<LightMask>(backLightMaskColorPtr.get(),
                                                                   backLightMaskResource->tintFactor));
        }
    }

    //侧面遮照（天光遮照）与点光源仅存在于地面层。
    if (layerType == TileLayerType::Ground) {
        const LightMaskResource *sideLightMaskResource = resourceLocator->FindLightMask(
            tileLightResourceData->GetSideLightMaskResource());
        if (sideLightMaskResource == nullptr) {
            // Tile has no side light mask resource, clear any existing side light mask data
            // 方块没有侧面光掩码资源，清除已有的侧面光掩码数据
            lightBuffer_->ClearLightMask(lightSourcePosition, layerType, LightDirection::Downward);
        } else {
            const std::unique_ptr<Color> sideLightMaskColorPtr = resourceLocator->FindColor(
                &sideLightMaskResource->lightMaskColor);
            if (sideLightMaskColorPtr == nullptr) {
                return;
            }
            if (sideLightMaskColorPtr->a == 0) {
                // Resource exists but has zero alpha - clear with re-propagation
                // 资源存在但alpha为0 - 清除并重新传播
                lightBuffer_->ClearLightMask(lightSourcePosition, layerType, LightDirection::Downward);
            } else {
                lightBuffer_->SetLightMask(lightSourcePosition, layerType, LightDirection::Downward,
                                           std::make_unique<LightMask>(sideLightMaskColorPtr.get(),
                                                                       sideLightMaskResource->tintFactor));
            }
        }
    }
    const LightSourceResource *lightSourceResource = resourceLocator->FindLightSource(
        tileLightResourceData->GetLightSourceResource());
    if (lightSourceResource == nullptr) {
        // Tile has no light source resource, clear existing light source if any
        // 方块没有光源资源，清除已有的光源
        lightBuffer_->ClearLightSource(lightSourcePosition, layerType);
    } else {
        const std::unique_ptr<Color> lightColorPtr = resourceLocator->
                FindColor(&lightSourceResource->lightColor);
        if (lightColorPtr == nullptr) {
            return;
        }
        if (lightColorPtr->a == 0) {
            LogCat::d(LogLabel::DEFAULT, "tile_light_source_zero_alpha",
                      "Tile light source has zero alpha, clearing: position=({}, {}), layer={}",
                      lightSourcePosition.x, lightSourcePosition.y, static_cast<int>(layerType));
            lightBuffer_->ClearLightSource(lightSourcePosition, layerType);
        } else {
            LogCat::d(LogLabel::DEFAULT, "tile_light_source_found",
                      "Found tile light source: position=({}, {}), layer={}, radius={}, rgba=({},{},{},{})",
                      lightSourcePosition.x, lightSourcePosition.y, static_cast<int>(layerType),
                      lightSourceResource->lightRadius,
                      static_cast<int>(lightColorPtr->r), static_cast<int>(lightColorPtr->g),
                      static_cast<int>(lightColorPtr->b), static_cast<int>(lightColorPtr->a));
            lightBuffer_->SetLightSource(lightSourcePosition, layerType,
                                         std::make_unique<LightSource>(
                                             lightSourcePosition, lightSourceResource->lightRadius,
                                             *lightColorPtr));
        }
    }
}


void glimmer::ChunkManager::UpdateChunkLight(const Chunk *chunk) const {
    LogCat::d(LogLabel::CHUNK, "chunk_update_light", "Updating chunk light: position=({}, {})",
              chunk->GetPosition().x,
              chunk->GetPosition().y);
    for (int index = 0; index < CHUNK_AREA; ++index) {
        for (int i = 0; i < TILE_LAYER_TYPE_COUNT; ++i) {
            UpdateTileLight(chunk, static_cast<TileLayerType>(1 << i), index);
        }
    }
}


glimmer::ChunkManager::ChunkManager(WorldContext *worldContext) : worldContext_(worldContext) {
    lightBuffer_ = std::make_unique<LightBuffer>();
    tileInstancePool_ = std::make_unique<TileInstancePool>();
    chunkLoader_ = worldContext_->GetChunkLoader();

    LogCat::i(LogLabel::CHUNK, "chunk_manager_created", "ChunkManager created for dimension folder.");
}

void glimmer::ChunkManager::LoadChunkAt(uint32_t maxChunksOccupiedByStructure,
                                        const ResourceRef &dimensionRef,
                                        const TileVector2D &position) {
    if (HasChunk(dimensionRef, position)) {
        return;
    }
    const AppContext *appContext = worldContext_->GetAppContext();
    if (appContext == nullptr) {
        return;
    }
    const ModContext *modContext = appContext->GetModContext();
    if (modContext == nullptr) {
        return;
    }
    const BiomeRegistry *biomeRegistry = modContext->GetBiomeRegistry();
    if (biomeRegistry == nullptr) {
        return;
    }
    const int worldSeed = worldContext_->GetWorldSeed();
    Dimension *dimension = worldContext_->GetDimension();
    if (dimension == nullptr) {
        return;
    }
    const DimensionResource *dimensionResource = dimension->GetDimensionResource();
    if (dimensionResource == nullptr) {
        return;
    }
    std::vector<std::shared_ptr<TerrainResult> > dependencyTerrain;

    if (TerrainGenerator *terrainGenerator = worldContext_->GetTerrainGenerator(); terrainGenerator != nullptr) {
        const std::vector<TileVector2D> dependencyTerrainPositions =
                StructureGeneratorManager::GetChunkDependencyTerrain(maxChunksOccupiedByStructure, position);
        dependencyTerrain.reserve(dependencyTerrainPositions.size());
        for (const TileVector2D &terrainPosition: dependencyTerrainPositions) {
            std::shared_ptr<TerrainResult> terrain = terrainGenerator->GenerateOrGetTerrain(biomeRegistry, worldSeed,
                dimensionResource, dimensionRef, terrainPosition);
            if (terrain == nullptr) {
                continue;
            }
            dependencyTerrain.emplace_back(std::move(terrain));
        }
    }
    LogCat::d(LogLabel::CHUNK, "chunk_loading", "Loading chunk at position: ({}, {})", position.x, position.y);
    std::unique_ptr<Chunk> newlyCreatedChunk = chunkLoader_->LoadChunkFromSaves(dimensionRef, position);
    if (newlyCreatedChunk == nullptr) {
        LogCat::d(LogLabel::CHUNK, "chunk_not_found_generating", "Chunk not found in saves, generating new chunk");
        newlyCreatedChunk = worldContext_->GetChunkGenerator()->GenerateChunkAt(position);
    }
    if (newlyCreatedChunk == nullptr) {
        LogCat::w(LogLabel::CHUNK, std::source_location::current(), "chunk_load_generate_failed",
                  "Failed to load or generate chunk at: ({}, {})", position.x,
                  position.y);
        return;
    }
    Chunk *chunkPtr = newlyCreatedChunk.get();
    newlyCreatedChunk->AddReplaceTileCallback([this](Chunk *chunk, const TileLayerType layerType,
                                                     const int index,
                                                     std::shared_ptr<Tile>, const std::shared_ptr<Tile> &newTile) {
        OnChunkTileChange(chunk, newTile, layerType, index);
    });
    newlyCreatedChunk->SetDependencyTerrain(std::move(dependencyTerrain));
    dimensionMap_[dimensionRef].insert({position, std::move(newlyCreatedChunk)});
    LogCat::d(LogLabel::CHUNK, "chunk_loaded", "Chunk loaded successfully at: ({}, {})", position.x, position.y);
    UpdateChunkLight(chunkPtr);
    ChunkPhysicsHelper::AttachPhysicsBodyToChunk(worldContext_->GetWorldId(), chunkPtr);
}

bool glimmer::ChunkManager::SaveChunk(const ResourceRef &dimensionRef, const TileVector2D &position) const {
    Chunk *chunk = GetChunk(dimensionRef, position);
    if (chunk == nullptr) {
        return false;
    }
    const Saves *saves = worldContext_->GetSaves();
    if (saves == nullptr) {
        return false;
    }
    LogCat::d(LogLabel::CHUNK, "chunk_saving", "Saving chunk: position=({}, {})", position.x, position.y);
    ChunkMessage chunkMessage;
    chunk->WriteChunkMessage(chunkMessage);
    if (!saves->WriteChunk(dimensionRef, position, chunkMessage)) {
        return false;
    }
    const WorldVector2D startWorldVector2d = chunk->GetStartWorldPosition();
    const WorldVector2D endWorldVector2d = chunk->GetEndWorldPosition();
    const float minX = std::min(startWorldVector2d.x, endWorldVector2d.x);
    const float maxX = std::max(startWorldVector2d.x, endWorldVector2d.x);
    const float minY = std::min(startWorldVector2d.y, endWorldVector2d.y);
    const float maxY = std::max(startWorldVector2d.y, endWorldVector2d.y);
    EntityManager *entityManager = worldContext_->GetEntityManager();
    auto transform2DEntities = entityManager->GetEntityIDWithComponents({COMPONENT_TRANSFORM_2D});
    ChunkEntityMessage chunkEntityMessage;
    std::vector<uint32_t> entitiesToRemove;
    const EntityShortCut *entityShortCut = worldContext_->GetEntityShortCut();
    if (entityShortCut == nullptr) {
        return false;
    }
    const GameEntityID player = entityShortCut->GetPlayer();
    for (auto &transform2dEntity: transform2DEntities) {
        if (transform2dEntity == player) {
            continue;
        }
        auto transform2dComponent = entityManager->GetComponent<Transform2DComponent>(transform2dEntity);
        if (transform2dComponent == nullptr) {
            continue;
        }
        const WorldVector2D &pos = transform2dComponent->GetPosition();
        if (pos.x < minX || pos.x >= maxX ||
            pos.y < minY || pos.y >= maxY) {
            continue;
        }
        if (entityManager->IsPersistable(transform2dEntity)) {
            worldContext_->SaveEntity(chunkEntityMessage.add_entities(), transform2dEntity);
        }
        //Whether this entity is successfully saved or not, it will disappear due to the block unloading.
        //无论这个实体是否成功保存，它都会因为区块卸载而消失。
        entitiesToRemove.emplace_back(transform2dEntity);
    }
    if (chunkEntityMessage.entities_size() > 0) {
        //Create a file and save it
        //创建文件并保存
        if (!saves->WriteChunkEntity(dimensionRef, position, chunkEntityMessage)) {
            return false;
        }
    } else {
        if (!saves->DeleteChunkEntity(dimensionRef, position)) {
            return false;
        }
    }
    for (const auto id: entitiesToRemove) {
        entityManager->RemoveEntity(id);
    }
    LogCat::d(LogLabel::CHUNK, "chunk_saved", "Chunk saved: position=({}, {}), entities={}", position.x, position.y,
              chunkEntityMessage.entities_size());
    return true;
}

size_t glimmer::ChunkManager::SaveAllChunk() {
    size_t chunkCount = 0;
    for (const auto &[dimensionRef, chunkMap]: dimensionMap_) {
        for (const auto &position: chunkMap | std::views::keys) {
            if (SaveChunk(dimensionRef, position)) {
                chunkCount++;
            }
        }
    }
    return chunkCount;
}


size_t glimmer::ChunkManager::GetLoadedChunkCount(const ResourceRef &dimensionRef) const {
    const auto dimensionIterator = dimensionMap_.find(dimensionRef);
    if (dimensionIterator == dimensionMap_.end()) {
        return 0;
    }
    return dimensionIterator->second.size();
}

const std::unordered_map<glimmer::TileVector2D, std::unique_ptr<glimmer::Chunk>, glimmer::Vector2DIHash> *glimmer::
ChunkManager::GetLoadedChunks(const ResourceRef &dimensionRef) const {
    const auto dimensionIterator = dimensionMap_.find(dimensionRef);
    if (dimensionIterator == dimensionMap_.end()) {
        return nullptr;
    }
    return &dimensionIterator->second;
}

glimmer::TileInstancePool *glimmer::ChunkManager::GetTileInstancePool() const {
    return tileInstancePool_.get();
}

bool glimmer::ChunkManager::ChunkIsOutOfBounds(const TileVector2D &position) {
    return position.y >= WORLD_MAX_Y || position.y < WORLD_MIN_Y || position.x >= WORLD_MAX_X || position.x <
           WORLD_MIN_X;
}

glimmer::LightBuffer *glimmer::ChunkManager::GetLightingBuffer() const {
    return lightBuffer_.get();
}
