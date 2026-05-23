//
// Created by Cold-Mint on 2025/12/29.
//

#include "DiggingSystem.h"
#include "core/world/TileInstancePool.h"
#include "core/mod/dataPack/TileResourceManager.h"
#include "core/world/WorldContext.h"
#include "../component/CameraComponent.h"
#include "../component/Transform2DComponent.h"
#include "../component/TileLayerComponent.h"
#include "../../inventory/TileItem.h"
#include "../../world/generator/Chunk.h"
#include "core/ecs/DroppedItemCreator.h"
#include "core/ecs/component/DiggingComponent.h"


bool glimmer::DiggingSystem::BreakTile(WorldContext *worldContext, const TileLayerComponent *tileLayerComponent,
                                       TileVector2D tilePosition, bool precisionMining, bool overwrite,
                                       ResourceRef newTileRef) {
    if (worldContext == nullptr || tileLayerComponent == nullptr) {
        return false;
    }
    const AppContext *appContext = worldContext->GetAppContext();
    if (appContext == nullptr) {
        return false;
    }
    const auto oldTile = tileLayerComponent->GetSelfLayerTilePtr(tilePosition);
    if (oldTile == nullptr) {
        return false;
    }
    if (overwrite && !oldTile->IsOverwritable()) {
        return false;
    }
    if (!overwrite && !oldTile->IsBreakable()) {
        return false;
    }
    TileStateMessage *tileStateMessage = tileLayerComponent->GetTileStatePtr(
        tileLayerComponent->GetTileLayerType(), tilePosition);
    ResourceRef oldResourceRef;
    oldResourceRef.ReadResourceRefMessage(tileStateMessage->resourceref());
    newTileRef.WriteResourceRefMessage(*tileStateMessage->mutable_resourceref());
    if (!tileLayerComponent->CommitTileState(tileLayerComponent->GetTileLayerType(), tilePosition, false)) {
        //If the placement fails, then revert to the previous resource reference.
        //如果放置失败了那么还原到之前的资源引用。
        oldResourceRef.WriteResourceRefMessage(*tileStateMessage->mutable_resourceref());
        return false;
    }
    auto *breakSFX = oldTile->GetBreakSFX();
    //No destructive sound effects will be played when it is covered.
    //被覆盖时不播放破坏音效。
    if (!overwrite && breakSFX != nullptr) {
        appContext->GetAudioManager()->TryPlayFree(AMBIENT, breakSFX, 0);
    }


    if (!oldTile->CanDropLoot()) {
        return true;
    }
    if (precisionMining || !oldTile->IsCustomLootTable()) {
        const GameEntity::ID droppedEntity = worldContext->CreateEntity();
        DroppedItemCreator droppedItemCreator{worldContext};
        droppedItemCreator.LoadTemplateComponents(droppedEntity, DroppedItemCreator::GetResourceRef());
        droppedItemCreator.MergeEntityItemMessage(droppedEntity, DroppedItemCreator::GetEntityItemMessage(
                                                      TileLayerComponent::TileToWorld(tilePosition),
                                                      std::make_unique<TileItem>(oldTile, oldResourceRef),
                                                      0));
        return true;
    }
    const auto lootResource = appContext->GetResourceLocator()->FindLoot(oldTile->GetLootTableRef());
    if (lootResource != nullptr) {
        std::vector<ItemMessage> itemMessageList = LootResource::GetLootItems(lootResource);
        for (auto &itemMessage: itemMessageList) {
            auto itemPtr = appContext->GetResourceLocator()->FindItem(worldContext, itemMessage);
            if (itemPtr == nullptr) {
                continue;
            }
            itemPtr->ReadItemMessage(worldContext, itemMessage);
            const GameEntity::ID droppedEntity = worldContext->CreateEntity();
            DroppedItemCreator droppedItemCreator{worldContext};
            droppedItemCreator.LoadTemplateComponents(droppedEntity, DroppedItemCreator::GetResourceRef());
            droppedItemCreator.MergeEntityItemMessage(droppedEntity, DroppedItemCreator::GetEntityItemMessage(
                                                          TileLayerComponent::TileToWorld(tilePosition),
                                                          std::move(itemPtr), 0));
        }
    }
    return true;
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
    const AppContext *appContext = worldContext_->GetAppContext();
    if (appContext == nullptr) {
        return;
    }
    const TileResourceManager *tileResourceManager = appContext->GetTileResourceManager();
    if (tileResourceManager == nullptr) {
        return;
    }
    TileInstancePool *tileInstancePool = worldContext_->GetTileInstancePool();
    if (tileInstancePool == nullptr) {
        return;
    }
    const auto tileLayerEntities = worldContext_->GetEntityIDWithComponents<TileLayerComponent>();
    for (auto &entity: tileLayerEntities) {
        const auto *tileLayer = worldContext_->GetComponent<TileLayerComponent>(entity);
        const TileLayerType tileLayerType = tileLayer->GetTileLayerType();
        if (tileLayerType != diggingComponent->GetLayerType()) {
            continue;
        }
        // Accumulate progress
        // 积累进度
        diggingComponent->AddProgress(
            diggingComponent->GetEfficiency() / diggingComponent->GetMiningRangeData()->GetMaxHardness() * delta);
        if (diggingComponent->GetProgress() >= 1.0F) {
            for (auto point: diggingComponent->GetMiningRangeData()->GetPoints()) {
                const TileVector2D tilePosition = TileLayerComponent::WorldToTile(point);
                BreakTile(worldContext_, tileLayer, tilePosition, diggingComponent->IsPrecisionMining(), false,
                          TileResourceManager::GetAirResourceRef(tileLayerType));
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
            resourceRef.SetResourceType(RESOURCE_TEXTURE);
            resourceRef.SetResourceKey("cracks/cracks_" + std::to_string(i));
            textureList.push_back(appContext->GetResourceLocator()->FindTexture(
                &resourceRef
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
