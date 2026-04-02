//
// Created by Cold-Mint on 2025/12/17.
//

#include "TileItem.h"
#include "../ecs/component/TileLayerComponent.h"
#include "../ecs/component/Transform2DComponent.h"
#include "../Constants.h"
#include "../world/generator/ChunkPhysicsHelper.h"
#include "../world/WorldContext.h"
#include "core/ecs/component/DebugDrawComponent.h"
#include "core/ecs/component/TilePlacementForbiddenZoneComponent.h"

glimmer::TileItem::TileItem(std::unique_ptr<Tile> tile) : tile_(std::move(tile)) {
    resourceRef_ = tile_->GetResourceRef();
    maxStack_ = ITEM_MAX_STACK;
}

const std::string &glimmer::TileItem::GetId() const {
    return tile_->GetId();
}

const std::string &glimmer::TileItem::GetName() const {
    return tile_->GetName();
}

const std::optional<std::string> &glimmer::TileItem::GetDescription() const {
    return tile_->GetDescription();
}

void glimmer::TileItem::OnUse(WorldContext *worldContext, GameEntity::ID user, const AbilityConfig &abilityConfig,
                              std::unordered_set<std::string> &popupAbility) {
    if (tile_ == nullptr) {
        return;
    }
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
            const std::string &currentTileId = currentTile->GetId();
            if (currentTileId == Resource::GenerateId(RESOURCE_REF_CORE, TILE_ID_AIR) || currentTileId ==
                Resource::GenerateId(
                    RESOURCE_REF_CORE, TILE_ID_WATER)) {
                bool isPlacementForbidden = false;
                const auto mobList = worldContext->GetEntityIDWithComponents<Transform2DComponent,
                    TilePlacementForbiddenZoneComponent>();
                auto targetWorldPos = TileLayerComponent::TileToWorldCenter(targetPos);
                for (uint32_t mob: mobList) {
                    const Transform2DComponent *transform2dComponent = worldContext->GetComponent<
                        Transform2DComponent>(mob);
                    if (transform2dComponent == nullptr) {
                        continue;
                    }
                    const TilePlacementForbiddenZoneComponent *forbiddenZone = worldContext->GetComponent<
                        TilePlacementForbiddenZoneComponent>(mob);
                    if (forbiddenZone == nullptr) {
                        continue;
                    }
                    //The rectangular area where tiles are prohibited from being placed.
                    //禁止放置瓦片的矩形范围。
                    float halfHeight = forbiddenZone->GetHeight() * HALF_TILE_SIZE;
                    float halfWidth = forbiddenZone->GetWidth() * HALF_TILE_SIZE;
                    WorldVector2D transform2dWorldPos = transform2dComponent->GetPosition();
                    auto forbiddenRect = SDL_FRect{
                        transform2dWorldPos.x - halfWidth + forbiddenZone->GetOffsetX() * TILE_SIZE,
                        transform2dWorldPos.y - halfHeight + forbiddenZone->GetOffsetY() * TILE_SIZE,
                        static_cast<float>(forbiddenZone->GetWidth() * TILE_SIZE),
                        static_cast<float>(forbiddenZone->GetHeight() * TILE_SIZE)
                    };
                    if (targetWorldPos.x >= forbiddenRect.x &&
                        targetWorldPos.x <= forbiddenRect.x + forbiddenRect.w &&
                        targetWorldPos.y >= forbiddenRect.y &&
                        targetWorldPos.y <= forbiddenRect.y + forbiddenRect.h) {
                        isPlacementForbidden = true;
                        break;
                    }
                }
                if (isPlacementForbidden) {
                    continue;
                }

                if (GetAmount() > 0) {
                    worldContext->GetAppContext()->GetAudioManager()->TryPlayFree(
                        AMBIENT, tile_->GetPlaceSFX(), 0);
                    auto tile = std::make_unique<Tile>(*tile_);
                    tile->SetPlayerPlaced(true);
                    (void) tileLayer->SetTile(
                        targetPos, std::move(tile));
                    auto chunk = Chunk::GetChunkByTileVector2D(worldContext->GetAllChunks(), targetPos);
                    if (chunk != nullptr) {
                        ChunkPhysicsHelper::UpdatePhysicsBodyToChunk(worldContext, chunk);
                    }
                    (void) RemoveAmount(1);
                }
            }
        }
    }
}


SDL_Texture *glimmer::TileItem::GetIcon() const {
    return tile_->GetTexture();
}

const glimmer::AbilityConfig &glimmer::TileItem::GetAbilityConfig() const {
    return abilityConfig_;
}

std::unique_ptr<glimmer::Item> glimmer::TileItem::Clone() const {
    return std::make_unique<TileItem>(std::make_unique<Tile>(*tile_));
}
