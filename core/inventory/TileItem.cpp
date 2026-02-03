//
// Created by Cold-Mint on 2025/12/17.
//

#include "TileItem.h"
#include "../ecs/component/TileLayerComponent.h"
#include "../ecs/component/Transform2DComponent.h"
#include "../Constants.h"
#include "../mod/ResourceLocator.h"
#include "../world/generator/ChunkPhysicsHelper.h"
#include "../world/WorldContext.h"

glimmer::TileItem::TileItem(std::unique_ptr<Tile> tile) : tile_(std::move(tile)) {
    maxStack_ = ITEM_MAX_STACK;
}

std::string glimmer::TileItem::GetId() const {
    return tile_->id;
}

std::string glimmer::TileItem::GetName() const {
    return tile_->name;
}

std::string glimmer::TileItem::GetDescription() const {
    return tile_->description;
}

void glimmer::TileItem::OnUse(AppContext *appContext, WorldContext *worldContext, GameEntity::ID user) {
    auto playerEntity = worldContext->GetPlayerEntity();
    if (WorldContext::IsEmptyEntityId(playerEntity)) {
        return;
    }
    auto playerTransform = worldContext->GetComponent<Transform2DComponent>(playerEntity);
    if (playerTransform == nullptr) {
        return;
    }
    const WorldVector2D playerWorldPos = playerTransform->GetPosition();
    const auto entities = worldContext->GetEntityIDWithComponents<TileLayerComponent>();
    for (auto &entity: entities) {
        auto *tileLayer = worldContext->GetComponent<TileLayerComponent>(entity);
        if (tileLayer->GetTileLayerType() == TileLayerType::Main) {
            auto targetPos = tileLayer->GetFocusPosition();
            if (TileLayerComponent::TileToWorldCenter(targetPos).Distance(playerWorldPos) / TILE_SIZE >
                TILE_PLACE_RANGE) {
                continue;
            }

            auto currentTile = tileLayer->GetTile(targetPos);
            if (currentTile == nullptr) {
                continue;
            }
            if (currentTile->id == Resource::GenerateId(RESOURCE_REF_CORE, TILE_ID_AIR) || currentTile->id ==
                Resource::GenerateId(
                    RESOURCE_REF_CORE, TILE_ID_WATER)) {
                if (GetAmount() > 0) {
                    auto resourceRef = Resource::ParseFromId(tile_->id, RESOURCE_TYPE_TILE);
                    if (resourceRef.has_value()) {
                        auto tileResource = appContext->GetResourceLocator()->FindTile(resourceRef.value());
                        if (tileResource.has_value()) {
                            (void) tileLayer->SetTile(
                                targetPos, Tile::FromResourceRef(appContext, tileResource.value()));
                            Chunk *chunk = Chunk::GetChunkByTileVector2D(worldContext->GetAllChunks(), targetPos);
                            if (chunk) {
                                ChunkPhysicsHelper::DetachPhysicsBodyToChunk(chunk);
                                ChunkPhysicsHelper::AttachPhysicsBodyToChunk(
                                    worldContext->GetWorldId(),
                                    chunk);
                            }
                        }
                    }
                    (void) RemoveAmount(1);
                }
            }
        }
    }
}

std::shared_ptr<SDL_Texture> glimmer::TileItem::GetIcon() const {
    return tile_->texture;
}

std::optional<glimmer::ResourceRef> glimmer::TileItem::ActualToResourceRef() {
    return Resource::ParseFromId(tile_->id, RESOURCE_TYPE_TILE);
}

std::unique_ptr<glimmer::Item> glimmer::TileItem::Clone() const {
    return std::make_unique<TileItem>(std::make_unique<Tile>(*tile_));
}
