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
#include "ChunkLoader.h"

#include <utility>

#include "Chunk.h"
#include "core/ecs/DroppedItemCreator.h"
#include "core/ecs/MobEntityCreator.h"
#include "core/ecs/component/AutoPickComponent.h"
#include "core/ecs/component/CameraComponent.h"
#include "core/ecs/component/DebugDrawComponent.h"
#include "core/ecs/component/DiggingComponent.h"
#include "core/ecs/component/DroppedItemComponent.h"
#include "core/ecs/component/ItemContainerComonent.h"
#include "core/ecs/component/MagnetComponent.h"
#include "core/ecs/component/MagneticComponent.h"
#include "core/ecs/component/RigidBody2DComponent.h"
#include "core/ecs/component/Transform2DComponent.h"
#include "core/log/LogCat.h"
#include "core/world/WorldContext.h"


void glimmer::ChunkLoader::LoadEntityFromSaves(TileVector2D position) const
{
    if (saves_->EntityExists(position))
    {
        auto chunkEntityMessageOptional = saves_->ReadChunkEntity(position);
        if (chunkEntityMessageOptional.has_value())
        {
            ChunkEntityMessage& chunkEntityMessage = chunkEntityMessageOptional.value();
            int entitySize = chunkEntityMessage.entities_size();
            for (int i = 0; i < entitySize; i++)
            {
                (void)RecoveryEntity(chunkEntityMessage.entities(i));
            }
        }
    }
}

glimmer::ChunkLoader::ChunkLoader(WorldContext* worldContext, Saves* saves) : saves_(saves), worldContext_(worldContext)
{
}

GameEntityID glimmer::ChunkLoader::RecoveryEntity(const EntityItemMessage& entityItemMessage) const
{
    const auto id = entityItemMessage.gameentity().id();
    worldContext_->GetEntityManager()->AddEntity(id);
    if (entityItemMessage.has_resourceref())
    {
        const ResourceRefMessage& resourceRefMessage = entityItemMessage.resourceref();
        const uint32_t resourceType = resourceRefMessage.resourcetype();
        if (resourceType == RESOURCE_MOB)
        {
            ResourceRef resourceRef{};
            resourceRef.ReadResourceRefMessage(resourceRefMessage);
            MobEntityCreator mobEntityCreator{worldContext_};
            mobEntityCreator.LoadTemplateComponents(id, resourceRef);
            mobEntityCreator.MergeEntityItemMessage(id, entityItemMessage);
        }

        if (resourceType == RESOURCE_DROPPED_ITEM)
        {
            ResourceRef resourceRef{};
            resourceRef.ReadResourceRefMessage(resourceRefMessage);
            DroppedItemCreator droppedItemCreator{worldContext_};
            droppedItemCreator.LoadTemplateComponents(id, resourceRef);
            droppedItemCreator.MergeEntityItemMessage(id, entityItemMessage);
        }
    }
    else
    {
        LogCat::e("The entity cannot be restored and there is a lack of resource references.");
    }
    return id;
}


std::unique_ptr<glimmer::Chunk> glimmer::ChunkLoader::LoadChunkFromSaves(TileVector2D position) const
{
    if (worldContext_ == nullptr)
    {
        return nullptr;
    }
    const AppContext* appContext = worldContext_->GetAppContext();
    if (appContext == nullptr)
    {
        return nullptr;
    }
    const Config* config = appContext->GetConfig();
    if (config == nullptr)
    {
        return nullptr;
    }
    if (saves_->ChunkExists(position))
    {
        //Read the chunk file.
        //读取区块文件。
        if (const auto chunkMessage = saves_->ReadChunk(position); chunkMessage.has_value())
        {
            auto chunk = std::make_unique<Chunk>(worldContext_, position, config->anim);
            chunk.get()->ReadChunkMessage(chunkMessage.value());
            LoadEntityFromSaves(position);
            return chunk;
        }
    }
    return nullptr;
}
