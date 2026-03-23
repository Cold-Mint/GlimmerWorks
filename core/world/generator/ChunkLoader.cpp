//
// Created by Cold-Mint on 2026/2/3.
//

#include "ChunkLoader.h"

#include <utility>

#include "Chunk.h"
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


glimmer::GameComponent *glimmer::ChunkLoader::RecoveryComponent(const GameEntity::ID id,
                                                                const ComponentMessage &componentMessage) const {
    uint32_t componentId = componentMessage.id();
    GameComponent *gameComponent = nullptr;
    switch (componentId) {
        case COMPONENT_ID_AUTO_PICK:
            gameComponent = worldContext_->AddComponent<AutoPickComponent>(id);
            break;
        case COMPONENT_ID_CAMERA:
            gameComponent = worldContext_->AddComponent<CameraComponent>(id);
            break;
        case COMPONENT_ID_DEBUG_DRAW:
            gameComponent = worldContext_->AddComponent<DebugDrawComponent>(id);
            break;
        case COMPONENT_ID_DIGGING:
            gameComponent = worldContext_->AddComponent<DiggingComponent>(id);
            break;
        case COMPONENT_ID_DROPPED_ITEM:
            gameComponent = worldContext_->AddComponent<DroppedItemComponent>(id);
            break;
        case COMPONENT_ID_ITEM_CONTAINER:
            gameComponent = worldContext_->AddComponent<ItemContainerComponent>(id);
            break;
        case COMPONENT_ID_MAGNET:
            gameComponent = worldContext_->AddComponent<MagnetComponent>(id);
            break;
        case COMPONENT_ID_MAGNETIC:
            gameComponent = worldContext_->AddComponent<MagneticComponent>(id);
            break;
        case COMPONENT_ID_RIGID_BODY_2D:
            gameComponent = worldContext_->AddComponent<RigidBody2DComponent>(id);
            break;
        case COMPONENT_ID_TRANSFORM_2D:
            gameComponent = worldContext_->AddComponent<Transform2DComponent>(id);
            break;
        default:
            LogCat::w("The game components do not support serialization.");
            return nullptr;
    }
    if (gameComponent != nullptr) {
        gameComponent->Deserialize(worldContext_, componentMessage.data());
    }
    return gameComponent;
}

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
    const int componentSize = entityItemMessage.components_size();
    for (int j = 0; j < componentSize; j++) {
        (void) RecoveryComponent(id, entityItemMessage.components(j));
    }

    if (entityItemMessage.has_resourceref()) {
        const ResourceRefMessage &resourceRefMessage = entityItemMessage.resourceref();
        if (resourceRefMessage.resourcetype() == RESOURCE_TYPE_MOB) {
            ResourceRef resourceRef;
            resourceRef.ReadResourceRefMessage(resourceRefMessage);
            const MobResource *mobResource = worldContext_->GetAppContext()->GetResourceLocator()->FindMob(resourceRef);
            if (mobResource == nullptr) {
                return GAME_ENTITY_ID_INVALID;
            }
            worldContext_->AttachMobRelatedComponents(id, mobResource);
            const Transform2DComponent *transform2dComponent = worldContext_->GetComponent<Transform2DComponent>(id);
            auto rigidBody2DComponent = worldContext_->GetComponent<RigidBody2DComponent>(id);
            if (transform2dComponent != nullptr && rigidBody2DComponent != nullptr) {
                rigidBody2DComponent->CreateBody(worldContext_->GetAppContext()->GetResourceLocator(),
                                                 worldContext_->GetWorldId(), transform2dComponent->GetPosition());
            }
        }
        if (resourceRefMessage.resourcetype() == RESOURCE_TYPE_DROPPED_ITEM) {
            // worldContext_->AttachDroppedItemRelatedComponents(id, std::move(returnItem));
            // const auto transform2dComponent = AddComponent<
            //     Transform2DComponent>(gameEntity);
            // if (transform2dComponent != nullptr) {
            //     transform2dComponent->SetPosition(playerPosition);
            // }
            // auto rigidBody2DComponent = GetComponent<RigidBody2DComponent>(gameEntity);
            // if (transform2dComponent != nullptr && rigidBody2DComponent != nullptr) {
            //     rigidBody2DComponent->CreateBody(GetAppContext()->GetResourceLocator(), worldId_,
            //                                      playerPosition);
            // }

            // worldContext_->AttachDroppedItemRelatedComponents(id, WorldVector2D(position.x(), position.y()), );
        }
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
