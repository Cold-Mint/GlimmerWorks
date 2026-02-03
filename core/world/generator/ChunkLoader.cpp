//
// Created by coldmint on 2026/2/3.
//

#include "ChunkLoader.h"

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
#include "core/utils/Box2DUtils.h"
#include "core/world/WorldContext.h"


glimmer::GameComponent *glimmer::ChunkLoader::RecoveryComponent(GameEntity::ID id,
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
    gameComponent->Deserialize(worldContext_, componentMessage.data());
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

glimmer::GameEntity::ID glimmer::ChunkLoader::RecoveryEntity(const EntityItemMessage &entityItemMessage) const {
    const auto id = entityItemMessage.gameentity().id();
    auto entity = std::make_unique<GameEntity>(id);
    const int componentSize = entityItemMessage.components_size();
    RigidBody2DComponent *rigidBody2dComponent = nullptr;
    Transform2DComponent *transform2dComponent = nullptr;
    for (int j = 0; j < componentSize; j++) {
        GameComponent *component = RecoveryComponent(id, entityItemMessage.components(j));
        if (component->GetId() == COMPONENT_ID_RIGID_BODY_2D) {
            rigidBody2dComponent = dynamic_cast<RigidBody2DComponent *>(component);
        }
        if (component->GetId() == COMPONENT_ID_TRANSFORM_2D) {
            transform2dComponent = dynamic_cast<Transform2DComponent *>(component);
        }
    }
    if (rigidBody2dComponent != nullptr && transform2dComponent != nullptr) {
        rigidBody2dComponent->CreateBody(worldContext_->GetWorldId(),
                                         Box2DUtils::ToMeters(
                                             transform2dComponent->GetPosition()));
    }
    return registerEntity_(std::move(entity));
}

glimmer::ChunkLoader::ChunkLoader(WorldContext *worldContext, Saves *saves,
                                  const std::function<bool(std::unique_ptr<GameEntity> entity)>
                                  &registerEntity) : saves_(saves), worldContext_(worldContext),
                                                     registerEntity_(registerEntity) {
}

std::unique_ptr<glimmer::Chunk> glimmer::ChunkLoader::LoadChunkFromSaves(TileVector2D position) const {
    if (saves_->ChunkExists(position)) {
        //Read the chunk file.
        //读取区块文件。
        if (const auto chunkMessage = saves_->ReadChunk(position); chunkMessage.has_value()) {
            auto chunk = std::make_unique<Chunk>(position);
            chunk.get()->FromMessage(worldContext_->GetAppContext(), chunkMessage.value());
            return chunk;
        }
    }
    LoadEntityFromSaves(position);
    return nullptr;
}
