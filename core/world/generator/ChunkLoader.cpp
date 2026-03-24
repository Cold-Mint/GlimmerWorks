//
// Created by Cold-Mint on 2026/2/3.
//

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


void glimmer::ChunkLoader::LoadEntityFromSaves(TileVector2D position) const {
    if (saves_->EntityExists(position)) {
        auto chunkEntityMessageOptional = saves_->ReadChunkEntity(position);
        if (chunkEntityMessageOptional.has_value()) {
            ChunkEntityMessage &chunkEntityMessage = chunkEntityMessageOptional.value();
            int entitySize = chunkEntityMessage.entitys_size();
            for (int i = 0; i < entitySize; i++) {
                (void) RecoveryEntity(chunkEntityMessage.entitys(i));
            }
        }
    }
}

glimmer::ChunkLoader::ChunkLoader(WorldContext *worldContext, Saves *saves,
                                  std::function<GameEntity::ID(std::unique_ptr<GameEntity> entity)>
                                  registerEntity) : saves_(saves), worldContext_(worldContext),
                                                    registerEntity_(std::move(registerEntity)) {
}

glimmer::GameEntity::ID glimmer::ChunkLoader::RecoveryEntity(const EntityItemMessage &entityItemMessage) const {
    const auto id = entityItemMessage.gameentity().id();
    auto entity = std::make_unique<GameEntity>(id);
    registerEntity_(std::move(entity));
    if (entityItemMessage.has_resourceref()) {
        const ResourceRefMessage &resourceRefMessage = entityItemMessage.resourceref();
        const uint32_t resourceType = resourceRefMessage.resourcetype();
        if (resourceType == RESOURCE_TYPE_MOB) {
            ResourceRef resourceRef{};
            resourceRef.ReadResourceRefMessage(resourceRefMessage);
            MobEntityCreator mobEntityCreator{worldContext_};
            mobEntityCreator.LoadTemplateComponents(id, resourceRef);
            mobEntityCreator.MergeEntityItemMessage(id, entityItemMessage);
        }

        if (resourceType == RESOURCE_TYPE_DROPPED_ITEM) {
            ResourceRef resourceRef{};
            resourceRef.ReadResourceRefMessage(resourceRefMessage);
            DroppedItemCreator droppedItemCreator{worldContext_};
            droppedItemCreator.LoadTemplateComponents(id, resourceRef);
            droppedItemCreator.MergeEntityItemMessage(id, entityItemMessage);
        }
    } else {
        LogCat::e("The entity cannot be restored and there is a lack of resource references.");
    }
    return id;
}


std::unique_ptr<glimmer::Chunk> glimmer::ChunkLoader::LoadChunkFromSaves(TileVector2D position) const {
    if (saves_->ChunkExists(position)) {
        //Read the chunk file.
        //读取区块文件。
        if (const auto chunkMessage = saves_->ReadChunk(position); chunkMessage.has_value()) {
            auto chunk = std::make_unique<Chunk>(position);
            chunk.get()->ReadChunkMessage(worldContext_->GetAppContext(), chunkMessage.value());
            LoadEntityFromSaves(position);
            return chunk;
        }
    }
    return nullptr;
}
