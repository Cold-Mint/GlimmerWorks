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
#include "CropSystem.h"

#include "core/context/AppContext.h"
#include "core/context/ModContext.h"
#include "core/ecs/component/CropComponent.h"
#include "core/log/LogCat.h"
#include "core/mod/GrowthConditionProcessorManager.h"
#include "core/mod/Resource.h"
#include "core/mod/ResourceRef.h"
#include "core/mod/dataPack/GrowthConditionsRegistry.h"
#include "core/world/ChunkManager.h"
#include "core/world/WorldContext.h"
#include "core/world/generator/Chunk.h"
#include "core/world/growth/IGrowthConditionProcessor.h"
#include "src/saves/tile_state.pb.h"

glimmer::CropSystem::CropSystem(WorldContext *worldContext) : GameSystem(worldContext) {
    WatchComponent(COMPONENT_CROP);
    Init();
}

void glimmer::CropSystem::OnWatchedComponentChanged(const GameComponentTypeMessage gameComponentType,
                                                    const uint32_t count) {
    if (gameComponentType != COMPONENT_CROP) {
        return;
    }
    cropCount_ = count;
    cropComponents_.clear();
    EntityManager *entityManager = GetEntityManager();
    if (entityManager == nullptr || cropCount_ == 0) {
        return;
    }
    const std::vector<GameEntityID> cropEntities = entityManager->GetEntityIDWithComponents({COMPONENT_CROP});
    for (const GameEntityID entity: cropEntities) {
        auto *cropComponent = entityManager->GetComponent<CropComponent>(entity);
        if (cropComponent != nullptr) {
            cropComponents_.emplace_back(cropComponent);
        }
    }
    LogCat::d("crop_system_entities_rebuilt", "CropSystem rebuilt entities: {}", cropComponents_.size());
}

void glimmer::CropSystem::OnGrowMature(Chunk *chunk, const TileVector2D &position, TileLayerType layerType,
                                       const ResourceRef *growthTargetRef) {
    //如果GetResourceType == RESOURCE_TILE,那么将当前Tile替换为目标Tile
    //如果 == RESOURCE_ABILITY_ITEM 或 RESOURCE_ABILITY_ITEM 或 RESOURCE_MATERIAL_ITEM 那么在当前位置生成掉落物
    //如果是 RESOURCE_LOOT_TABLE 那么抽取战利品表，并且将结果以掉落物的形式生成在当前位置
//如果是 RESOURCE_STRUCTURE 那么在当前位置生成结构
}


void glimmer::CropSystem::OnTick(const uint64_t tick) {
    WorldContext *worldContext = GetWorldContext();
    if (worldContext == nullptr || cropComponents_.empty()) {
        return;
    }
    ChunkManager *chunkManager = worldContext->GetChunkManager();
    const AppContext *appContext = worldContext->GetAppContext();
    if (chunkManager == nullptr || appContext == nullptr) {
        return;
    }
    ModContext *modContext = appContext->GetModContext();
    if (modContext == nullptr) {
        return;
    }
    GrowthConditionProcessorManager *growthConditionProcessorManager = modContext->
            GetGrowthConditionProcessorManager();
    GrowthConditionsRegistry *growthConditionsRegistry = modContext->GetGrowthConditionsRegistry();
    if (growthConditionProcessorManager == nullptr || growthConditionsRegistry == nullptr) {
        return;
    }
    for (CropComponent *cropComponent: cropComponents_) {
        if (cropComponent == nullptr) {
            continue;
        }
        const TileVector2D &position = cropComponent->GetPosition();
        const TileLayerType layerType = cropComponent->GetLayerType();
        Chunk *chunk = chunkManager->GetChunk(Chunk::TileCoordinatesToChunkVertexCoordinates(position));
        if (chunk == nullptr) {
            continue;
        }
        const TileVector2D relative = Chunk::TileCoordinatesToChunkRelativeCoordinates(position);
        const int index = relative.y << CHUNK_SHIFT | relative.x;
        const Tile *tile = chunk->GetTile(layerType, index);
        if (tile == nullptr || !tile->IsCropsBlock()) {
            continue;
        }

        bool conditionsMet = true;
        for (const ResourceRef &conditionRef: tile->GetGrowthConditions()) {
            IGrowthConditionResource *conditionResource = growthConditionsRegistry->Find(
                conditionRef.GetPackageId(), conditionRef.GetResourceKey());
            if (conditionResource == nullptr) {
                conditionsMet = false;
                break;
            }
            const auto processorType = static_cast<GrowthConditionProcessorType>(conditionResource->processorId);
            IGrowthConditionProcessor *processor = growthConditionProcessorManager->FindConditionProcessors(
                processorType);
            if (processor == nullptr) {
                conditionsMet = false;
                break;
            }
            if (!processor->Match(worldContext, position, conditionResource)) {
                conditionsMet = false;
                break;
            }
        }
        if (!conditionsMet) {
            continue;
        }

        TileStateMessage *tileState = chunk->GetOrCreateTileState(layerType, index);
        if (tileState == nullptr) {
            continue;
        }
        tileState->set_tilegrowthaccumulatedtick(tileState->tilegrowthaccumulatedtick() + 1);
        if (tileState->tilegrowthrequiredtick() < tileState->tilegrowthaccumulatedtick()) {
            continue;
        }
        const ResourceRef *growthTargetRef = tile->GetGrowthTarget();
        if (growthTargetRef == nullptr) {
            continue;
        }
        OnGrowMature(chunk, relative, layerType, growthTargetRef);
    }
}

glimmer::GameSystemType glimmer::CropSystem::GetGameSystemType() const {
    return GameSystemType::CropSystem;
}
