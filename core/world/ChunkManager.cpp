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

#include "core/Constants.h"
#include "core/ecs/EntityManager.h"
#include "core/ecs/EntityShortCut.h"
#include "core/ecs/component/Transform2DComponent.h"
#include "core/log/LogCat.h"
#include "core/mod/ResourceLocator.h"
#include "core/saves/Saves.h"
#include "core/scene/AppContext.h"
#include "core/world/Tile.h"
#include "core/world/WorldContext.h"
#include "generator/ChunkGenerator.h"
#include "generator/ChunkLoader.h"
#include "generator/ChunkPhysicsHelper.h"
#include "src/saves/chunk.pb.h"

glimmer::ChunkManager::ChunkManager(WorldContext* worldContext) : worldContext_(worldContext)
{
    lightBuffer_ = std::make_unique<LightBuffer>();
    tileInstancePool_ = std::make_unique<TileInstancePool>();
}

void glimmer::ChunkManager::OnChunkTileChange(Chunk* chunk, [[maybe_unused]] const std::shared_ptr<Tile>& tile,
                                              TileLayerType layerType, int index) const
{
    if (layerType == TileLayerType::Ground)
    {
        ChunkPhysicsHelper::UpdatePhysicsBodyToChunk(worldContext_, chunk);
    }
    UpdateTileLight(chunk, layerType, index);
}


void glimmer::ChunkManager::UpdateTileLight(const Chunk* chunk, const TileLayerType layerType, const int index) const
{
    const AppContext* appContext = worldContext_->GetAppContext();
    if (appContext == nullptr)
    {
        return;
    }
#if  !defined(NDEBUG)
    const Config* config = appContext->GetConfig();
    if (config == nullptr)
    {
        return;
    }
    if (!config->light.enable)
    {
        return;
    }
#endif
    if (chunk == nullptr)
    {
        return;
    }
    const Tile* tile = chunk->GetTile(layerType, index);
    if (tile == nullptr)
    {
        return;
    }
    const ResourceLocator* resourceLocator = appContext->GetResourceLocator();
    if (resourceLocator == nullptr)
    {
        return;
    }
    const TileVector2D chunkPosition = chunk->GetPosition();
    const int localX = index & CHUNK_MASK;
    const int localY = index >> CHUNK_SHIFT;
    auto lightSourcePosition = TileVector2D(chunkPosition.x + localX, chunkPosition.y + localY);
    const LightMaskResource* sideLightMaskResource = resourceLocator->FindLightMask(tile->GetSideLightMaskResource());
    if (sideLightMaskResource != nullptr)
    {
        const std::unique_ptr<Color> sideLightMaskColorPtr = resourceLocator->FindColor(
            &sideLightMaskResource->lightMaskColor);
        if (sideLightMaskColorPtr == nullptr)
        {
            return;
        }
        if (sideLightMaskColorPtr->a == 0)
        {
            lightBuffer_->ClearSideLightMask(lightSourcePosition, tile->GetLayerType());
        }
        else
        {
            lightBuffer_->SetSideLightMask(lightSourcePosition, layerType,
                                           std::make_unique<LightMask>(sideLightMaskColorPtr.get(),
                                                                       sideLightMaskResource->tintFactor));
        }
    }
    const LightMaskResource* backLightMaskResource = resourceLocator->FindLightMask(tile->GetBackLightMaskResource());
    if (backLightMaskResource != nullptr)
    {
        const std::unique_ptr<Color> backLightMaskColorPtr = resourceLocator->FindColor(
            &backLightMaskResource->lightMaskColor);
        if (backLightMaskColorPtr == nullptr)
        {
            return;
        }
        if (backLightMaskColorPtr->a == 0)
        {
            lightBuffer_->ClearBackLightMask(lightSourcePosition, tile->GetLayerType());
        }
        else
        {
            lightBuffer_->SetBackLightMask(lightSourcePosition, layerType,
                                           std::make_unique<LightMask>(backLightMaskColorPtr.get(),
                                                                       backLightMaskResource->tintFactor));
        }
    }
    const LightSourceResource* lightSourceResource = resourceLocator->FindLightSource(
        tile->GetLightSourceResource());
    if (lightSourceResource != nullptr)
    {
        const std::unique_ptr<Color> lightColorPtr = resourceLocator->
            FindColor(&lightSourceResource->lightColor);
        if (lightColorPtr == nullptr)
        {
            return;
        }
        if (lightColorPtr->a == 0)
        {
            lightBuffer_->ClearLightSource(lightSourcePosition, layerType);
        }
        else
        {
            lightBuffer_->SetLightSource(lightSourcePosition, layerType,
                                         std::make_unique<LightSource>(
                                             lightSourcePosition, lightSourceResource->lightRadius,
                                             *lightColorPtr));
        }
    }
}


void glimmer::ChunkManager::UpdateChunkLight(const Chunk* chunk) const
{
    for (int index = 0; index < CHUNK_AREA; ++index)
    {
        for (int i = 0; i < TILE_LAYER_TYPE_COUNT; ++i)
        {
            UpdateTileLight(chunk, static_cast<TileLayerType>(1 << i), index);
        }
    }
}

std::unordered_map<glimmer::TileVector2D, glimmer::Chunk*, glimmer::Vector2DIHash>*
glimmer::ChunkManager::GetAllChunks()
{
    if (lastChunkSnapshot_ != chunkSnapshot_)
    {
        chunksCache_.clear();
        chunksCache_.reserve(chunks_.size());

        for (const auto& [pos, chunkPtr] : chunks_)
        {
            chunksCache_[pos] = chunkPtr.get();
        }
        lastChunkSnapshot_ = chunkSnapshot_;
    }
    return &chunksCache_;
}

void glimmer::ChunkManager::LoadChunkAt(TileVector2D position)
{
    if (chunks_.contains(position))
    {
        return;
    }
    std::unique_ptr<Chunk> newlyCreatedChunk = worldContext_->GetChunkLoader()->LoadChunkFromSaves(position);
    if (newlyCreatedChunk == nullptr)
    {
        newlyCreatedChunk = worldContext_->GetChunkGenerator()->GenerateChunkAt(position);
    }
    if (newlyCreatedChunk == nullptr)
    {
        return;
    }
    UpdateChunkLight(newlyCreatedChunk.get());
    ChunkPhysicsHelper::AttachPhysicsBodyToChunk(worldContext_->GetAppContext(), worldContext_->GetWorldId(),
                                                 newlyCreatedChunk.get());
    newlyCreatedChunk->AddReplaceTileCallback([this](Chunk* chunk, TileLayerType layerType,
                                                     int index,
                                                     std::shared_ptr<Tile>, const std::shared_ptr<Tile>& newTile)
    {
        OnChunkTileChange(chunk, newTile, layerType, index);
    });
    chunks_.insert({position, std::move(newlyCreatedChunk)});
    chunkSnapshot_++;
}


void glimmer::ChunkManager::UnloadChunkAt(const TileVector2D& position)
{
    auto it = chunks_.find(position);
    if (it == chunks_.end())
    {
        return;
    }
    if (SaveChunk(position))
    {
        //Uninstall the light sources and shadows of the current chunk.
        //卸载当前区块的光源和遮挡。
        for (int x = 0; x < CHUNK_SIZE; x++)
        {
            for (int y = 0; y < CHUNK_SIZE; y++)
            {
                lightBuffer_->ClearTileLightData(TileVector2D(position.x + x, position.y + y));
            }
        }
        ChunkPhysicsHelper::DetachPhysicsBodyToChunk(worldContext_->GetAppContext(), it->second.get());
        chunks_.erase(it);
        chunkSnapshot_++;
    }
    else
    {
        LogCat::e("Saving the chunk failed, x=", position.x, ",y=", position.y);
    }
}

glimmer::Chunk* glimmer::ChunkManager::GetChunk(const TileVector2D position)
{
#if  !defined(NDEBUG)
    const TileVector2D relativeVector = Chunk::TileCoordinatesToChunkRelativeCoordinates(position);
    if (relativeVector.x != 0 || relativeVector.y != 0)
    {
        LogCat::e("The coordinates are not the vertices of the chunk.");
        assert(false);
    }
#endif

    const auto it = chunks_.find(position);
    if (it == chunks_.end())
    {
        return nullptr;
    }
    return it->second.get();
}

glimmer::TileInstancePool* glimmer::ChunkManager::GetTileInstancePool() const
{
    return tileInstancePool_.get();
}

bool glimmer::ChunkManager::SaveChunk(TileVector2D position)
{
    const auto it = chunks_.find(position);
    if (it == chunks_.end())
    {
        return false;
    }
    ChunkMessage chunkMessage;
    Chunk* chunk = it->second.get();
    chunk->WriteChunkMessage(chunkMessage);
    (void)worldContext_->GetSaves()->WriteChunk(position, chunkMessage);
    const WorldVector2D startWorldVector2d = chunk->GetStartWorldPosition();
    const WorldVector2D endWorldVector2d = chunk->GetEndWorldPosition();
    const float minX = std::min(startWorldVector2d.x, endWorldVector2d.x);
    const float maxX = std::max(startWorldVector2d.x, endWorldVector2d.x);
    const float minY = std::min(startWorldVector2d.y, endWorldVector2d.y);
    const float maxY = std::max(startWorldVector2d.y, endWorldVector2d.y);
    EntityManager* entityManager = worldContext_->GetEntityManager();
    auto transform2DEntities = entityManager->GetEntityIDWithComponents({COMPONENT_TRANSFORM_2D});
    ChunkEntityMessage chunkEntityMessage;
    std::vector<uint32_t> entitiesToRemove;
    GameEntityID player = worldContext_->GetEntityShortCut()->GetPlayer();
    for (auto& transform2dEntity : transform2DEntities)
    {
        if (transform2dEntity == player)
        {
            continue;
        }
        auto transform2dComponent = entityManager->GetComponent<Transform2DComponent>(transform2dEntity);
        if (transform2dComponent == nullptr)
        {
            continue;
        }
        WorldVector2D pos = transform2dComponent->GetPosition();
        if (pos.x < minX || pos.x >= maxX ||
            pos.y < minY || pos.y >= maxY)
        {
            continue;
        }
        if (entityManager->IsPersistable(transform2dEntity))
        {
            worldContext_->SaveEntity(chunkEntityMessage.add_entities(), transform2dEntity);
        }
        //Whether this entity is successfully saved or not, it will disappear due to the block unloading.
        //无论这个实体是否成功保存，它都会因为区块卸载而消失。
        entitiesToRemove.emplace_back(transform2dEntity);
    }
    if (chunkEntityMessage.entities_size() > 0)
    {
        //Create a file and save it
        //创建文件并保存
        (void)worldContext_->GetSaves()->WriteChunkEntity(position, chunkEntityMessage);
    }
    else
    {
        (void)worldContext_->GetSaves()->DeleteChunkEntity(position);
    }
    for (auto id : entitiesToRemove)
    {
        entityManager->RemoveEntity(id);
    }
    return true;
}

bool glimmer::ChunkManager::HasChunk(const TileVector2D position) const
{
    return chunks_.contains(position);
}

bool glimmer::ChunkManager::ChunkIsOutOfBounds(const TileVector2D position)
{
    return position.y >= WORLD_MAX_Y || position.y < WORLD_MIN_Y || position.x >= WORLD_MAX_X || position.x <
        WORLD_MIN_X;
}

glimmer::LightBuffer* glimmer::ChunkManager::GetLightingBuffer() const
{
    return lightBuffer_.get();
}
