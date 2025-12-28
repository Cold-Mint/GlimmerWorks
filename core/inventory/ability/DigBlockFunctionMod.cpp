//
// Created by coldmint on 2025/12/25.
//

#include "DigBlockFunctionMod.h"
#include "../../world/WorldContext.h"
#include "../../ecs/component/TileLayerComponent.h"
#include "../../ecs/component/Transform2DComponent.h"
#include "../../inventory/TileItem.h"
#include "../../world/ChunkPhysicsHelper.h"

std::string glimmer::DigBlockFunctionMod::GetId() const {
    return "glimmer.DigBlockFunctionMod";
}

void glimmer::DigBlockFunctionMod::OnUse(AppContext *appContext, WorldContext *worldContext, GameEntity *user) {
    auto tileLayerEntitys = worldContext->GetEntitiesWithComponents<
        TileLayerComponent, Transform2DComponent>();
    for (auto &gameEntity: tileLayerEntitys) {
        auto *tileLayerComponent = worldContext->GetComponent<TileLayerComponent>(
            gameEntity->GetID());
        auto *tileLayerTransform2D = worldContext->GetComponent<Transform2DComponent>(
            gameEntity->GetID());
        if (tileLayerComponent == nullptr || tileLayerTransform2D == nullptr) {
            continue;
        }
        if (tileLayerComponent->GetTileLayerType() == TileLayerType::Main) {
            TileVector2D tileVector2D = tileLayerComponent->GetFocusPosition();
            const Tile *tile = tileLayerComponent->GetTile(
                tileVector2D);
            if (tile == nullptr) {
                continue;
            }
            if (!tile->breakable) {
                continue;
            }
            auto oldTile = tileLayerComponent->ReplaceTile(tileVector2D,
                                                           Tile::FromResourceRef(
                                                               appContext,
                                                               appContext->GetTileManager()->GetAir()));

            worldContext->CreateDroppedItemEntity(std::make_unique<TileItem>(std::move(oldTile)),
                                                   TileLayerComponent::TileToWorld(
                                                       tileLayerTransform2D->GetPosition(), tileVector2D));
            const auto chunk = Chunk::GetChunkByTileVector2D(worldContext->GetAllChunks(), tileVector2D);
            if (chunk == nullptr) {
                continue;
            }
            ChunkPhysicsHelper::DetachPhysicsBodyToChunk(chunk);
            ChunkPhysicsHelper::AttachPhysicsBodyToChunk(worldContext->GetWorldId(),
                                                         tileLayerTransform2D->GetPosition(),
                                                         chunk);
        }
    }
}