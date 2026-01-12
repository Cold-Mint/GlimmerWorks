//
// Created by Cold-Mint on 2025/12/17.
//

#include "TileItem.h"
#include "../ecs/component/TileLayerComponent.h"
#include "../ecs/component/Transform2DComponent.h"
#include "../Constants.h"
#include "../mod/ResourceLocator.h"
#include "../world/ChunkPhysicsHelper.h"
#include "../world/WorldContext.h"

std::string glimmer::TileItem::GetId() const {
    return tile_->id;
}

std::string glimmer::TileItem::GetName() const {
    return tile_->name;
}

std::string glimmer::TileItem::GetDescription() const {
    return tile_->description;
}

void glimmer::TileItem::OnUse(AppContext *appContext, WorldContext *worldContext, GameEntity *user) {
    const auto entities = worldContext->GetEntitiesWithComponents<TileLayerComponent>();
    for (auto &entity: entities) {
        auto *tileLayer = worldContext->GetComponent<TileLayerComponent>(entity->GetID());
        if (tileLayer->GetTileLayerType() == TileLayerType::Main) {
            auto targetPos = tileLayer->GetFocusPosition();
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

std::optional<glimmer::ResourceRef> glimmer::TileItem::ToResourceRef() {
    return Resource::ParseFromId(tile_->id, RESOURCE_TYPE_TILE);
}

std::unique_ptr<glimmer::Item> glimmer::TileItem::Clone() const {
    return std::make_unique<TileItem>(std::make_unique<Tile>(*tile_));
}
