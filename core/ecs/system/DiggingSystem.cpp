//
// Created by Cold-Mint on 2025/12/29.
//

#include "DiggingSystem.h"

#include "../../world/WorldContext.h"
#include "../component/CameraComponent.h"
#include "../component/Transform2DComponent.h"
#include "../component/TileLayerComponent.h"
#include "../../inventory/TileItem.h"
#include "../../world/generator/Chunk.h"
#include "core/ecs/DroppedItemCreator.h"
#include "core/ecs/component/DiggingComponent.h"
#include "core/world/generator/ChunkPhysicsHelper.h"

void glimmer::DiggingSystem::BreakTile(const TileVector2D tilePosition, const AppContext *appContext,
                                       const DiggingComponent *diggingComponent,
                                       const TileLayerComponent *tileLayerComponent) const {
    const TileManager *tileManager = appContext->GetTileManager();
    auto oldTile = tileLayerComponent->ReplaceTile(
        tilePosition, Tile::FromTileResource(appContext, tileManager->GetAir(),
                                             *tileManager->GetAirResourceRef()));

    if (oldTile == nullptr) {
        return;
    }
    auto *breakSFX = oldTile->GetBreakSFX();
    if (breakSFX != nullptr) {
        appContext->GetAudioManager()->TryPlayFree(AMBIENT, breakSFX, 0);
    }
    if (!diggingComponent->IsPrecisionMining() && oldTile->IsCustomLootTable()) {
        const auto lootResource = appContext->GetResourceLocator()->FindLoot(oldTile->GetLootTableRef());
        if (lootResource != nullptr) {
            std::vector<ItemMessage> itemMessageList = LootResource::GetLootItems(lootResource);
            for (auto &itemMessage: itemMessageList) {
                auto itemPtr = appContext->GetResourceLocator()->FindItem(itemMessage);
                if (itemPtr == nullptr) {
                    continue;
                }
                itemPtr->ReadItemMessage(appContext, itemMessage);
                const GameEntity::ID droppedEntity = worldContext_->CreateEntity();
                DroppedItemCreator droppedItemCreator{worldContext_};
                droppedItemCreator.LoadTemplateComponents(droppedEntity, DroppedItemCreator::GetResourceRef());
                droppedItemCreator.MergeEntityItemMessage(droppedEntity, DroppedItemCreator::GetEntityItemMessage(
                                                              TileLayerComponent::TileToWorld(tilePosition),
                                                              std::move(itemPtr), 0));
            }
        }
    } else {
        const GameEntity::ID droppedEntity = worldContext_->CreateEntity();
        DroppedItemCreator droppedItemCreator{worldContext_};
        droppedItemCreator.LoadTemplateComponents(droppedEntity, DroppedItemCreator::GetResourceRef());
        droppedItemCreator.MergeEntityItemMessage(droppedEntity, DroppedItemCreator::GetEntityItemMessage(
                                                      TileLayerComponent::TileToWorld(tilePosition),
                                                      std::make_unique<TileItem>(
                                                          std::move(oldTile)),
                                                      0));
    }
}

glimmer::DiggingSystem::DiggingSystem(WorldContext *worldContext) : GameSystem(worldContext) {
    RequireComponent<DiggingComponent>();
}

void glimmer::DiggingSystem::Update(float delta) {
    if (worldContext_ == nullptr) {
        return;
    }
    auto diggingComponent = worldContext_->GetDiggingComponent();
    if (!diggingComponent->CheckAndResetActive()) {
        diggingComponent->SetEnable(false);
        diggingComponent->SetProgress(0.0F);
        return;
    }
    diggingComponent->SetEnable(true);
    AppContext *appContext = worldContext_->GetAppContext();
    const auto tileLayerEntities = worldContext_->GetEntityIDWithComponents<TileLayerComponent>();
    for (auto &entity: tileLayerEntities) {
        const auto *tileLayer = worldContext_->GetComponent<TileLayerComponent>(entity);
        if (tileLayer->GetTileLayerType() != diggingComponent->GetLayerType()) {
            continue;
        }
        // Accumulate progress
        // 积累进度
        diggingComponent->AddProgress(
            diggingComponent->GetEfficiency() / diggingComponent->GetMiningRangeData()->GetMaxHardness() * delta);
        if (diggingComponent->GetProgress() >= 1.0F) {
            // Break the tile
            std::unordered_set<Chunk *> chunkSet;
            for (auto point: diggingComponent->GetMiningRangeData()->GetPoints()) {
                const TileVector2D tilePosition = TileLayerComponent::WorldToTile(point);
                Chunk *chunk = worldContext_->GetChunk(Chunk::TileCoordinatesToChunkVertexCoordinates(tilePosition));
                if (chunk != nullptr) {
                    chunkSet.insert(chunk);
                }
                BreakTile(tilePosition, appContext, diggingComponent, tileLayer);
            }
            // Update physics
            // 更新物理
            if (diggingComponent->GetLayerType() == Ground) {
                for (auto chunk: chunkSet) {
                    if (chunk == nullptr) {
                        continue;
                    }
                    ChunkPhysicsHelper::UpdatePhysicsBodyToChunk(worldContext_, chunk);
                }
            }
            // Reset digging after break
            // 破坏方块重置挖掘
            diggingComponent->SetProgress(0.0F);
            diggingComponent->SetEnable(false);
        }
        break;
    }
}

void glimmer::DiggingSystem::Render(SDL_Renderer *renderer) {
    AppContext *appContext = worldContext_->GetAppContext();
    if (!cacheTexture) {
        for (uint8_t i = 0; i < 10; i++) {
            ResourceRef resourceRef;
            resourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
            resourceRef.SetResourceType(RESOURCE_TYPE_TEXTURES);
            resourceRef.SetResourceKey("cracks/cracks_" + std::to_string(i));
            textureList.push_back(appContext->GetResourceLocator()->FindTexture(
                resourceRef
            ));
        }
        cacheTexture = true;
        return;
    }
    auto diggingComponent = worldContext_->GetDiggingComponent();
    if (!diggingComponent->IsEnable()) {
        return;
    }
    auto cameraTransform2D = worldContext_->GetCameraTransform2D();
    auto cameraComponent = worldContext_->GetCameraComponent();
    for (auto point: diggingComponent->GetMiningRangeData()->GetPoints()) {
        if (!cameraComponent->IsPointInViewport(cameraTransform2D->GetPosition(), point)) {
            return;
        }
        CameraVector2D cameraVector2d = cameraComponent->GetViewPortPosition(
            cameraTransform2D->GetPosition(), point);
        float size = TILE_SIZE * cameraComponent->GetZoom();

        const auto maxIndex = static_cast<float>(textureList.size() - 1);
        uint8_t crackIndex = static_cast<uint8_t>(std::min(diggingComponent->GetProgress() * maxIndex, maxIndex));
        SDL_FRect dstRect = {cameraVector2d.x - size / 2, cameraVector2d.y - size / 2, size, size};
        auto &crackTexture = textureList[crackIndex];
        if (crackTexture) {
            SDL_RenderTexture(renderer, crackTexture.get(), nullptr, &dstRect);
        }
    }
}

uint8_t glimmer::DiggingSystem::GetRenderOrder() {
    return RENDER_ORDER_DIGGING;
}

std::string glimmer::DiggingSystem::GetName() {
    return "glimmer.DiggingSystem";
}
