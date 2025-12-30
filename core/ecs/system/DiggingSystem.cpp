//
// Created by coldmint on 2025/12/29.
//

#include "DiggingSystem.h"

#include "../../world/WorldContext.h"
#include "../component/CameraComponent.h"
#include "../component/Transform2DComponent.h"
#include "../component/TileLayerComponent.h"
#include "../../inventory/TileItem.h"
#include "../../world/ChunkPhysicsHelper.h"
#include "../../world/Chunk.h"

void glimmer::DiggingSystem::Update(float delta) {
    auto diggingComponent = worldContext_->GetDiggingComponent();
    if (!diggingComponent->CheckAndResetActive()) {
        diggingComponent->SetEnable(false);
        diggingComponent->SetProgress(0.0F);
        return;
    }
    diggingComponent->SetEnable(true);

    const auto tileLayerEntities = worldContext_->GetEntitiesWithComponents<TileLayerComponent, Transform2DComponent>();
    for (auto &entity: tileLayerEntities) {
        auto *tileLayer = worldContext_->GetComponent<TileLayerComponent>(entity->GetID());
        auto *transform = worldContext_->GetComponent<Transform2DComponent>(entity->GetID());

        if (tileLayer->GetTileLayerType() != diggingComponent->GetLayerType()) {
            continue;
        }

        TileVector2D tilePos = TileLayerComponent::WorldToTile(transform->GetPosition(),
                                                               diggingComponent->GetPosition());
        Tile *tile = tileLayer->GetTile(tilePos);
        if (tile == nullptr) {
            continue;
        }
        if (!tile->breakable) {
            continue;
        }
        // Accumulate progress
        // 积累进度
        diggingComponent->AddProgress(diggingComponent->GetEfficiency() / tile->hardness * delta);
        LogCat::d("efficiency=", diggingComponent->GetEfficiency(), " ,hardness=", tile->hardness,
                  " ,progress=", diggingComponent->GetProgress());

        if (diggingComponent->GetProgress() >= 1.0F) {
            // Break the tile
            auto oldTile = tileLayer->ReplaceTile(
                tilePos, Tile::FromResourceRef(appContext_, appContext_->GetTileManager()->GetAir()));

            if (oldTile) {
                worldContext_->CreateDroppedItemEntity(
                    std::make_unique<TileItem>(std::move(oldTile)),
                    TileLayerComponent::TileToWorld(transform->GetPosition(), tilePos)
                );
            }

            // Update physics
            // 更新物理
            Chunk *chunk = Chunk::GetChunkByTileVector2D(worldContext_->GetAllChunks(), tilePos);
            if (chunk) {
                ChunkPhysicsHelper::DetachPhysicsBodyToChunk(chunk);
                ChunkPhysicsHelper::AttachPhysicsBodyToChunk(worldContext_->GetWorldId(), transform->GetPosition(),
                                                             chunk);
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
    if (!cacheTexture) {
        for (uint8_t i = 0; i < 10; i++) {
            std::shared_ptr<SDL_Texture> texture = appContext_->GetResourcePackManager()->LoadTextureFromFile(
                appContext_,
                "cracks/cracks_" + std::to_string(i) + ".png");
            if (texture == nullptr) {
                LogCat::e("Failed to load cracks texture: " + std::to_string(i));
                continue;
            }
            textureList.push_back(texture);
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

    if (!cameraComponent->IsPointInViewport(cameraTransform2D->GetPosition(), diggingComponent->GetPosition())) {
        LogCat::w("DiggingSystem::Render: Point is not in viewport");
        return;
    }
    CameraVector2D cameraVector2d = cameraComponent->GetViewPortPosition(
        cameraTransform2D->GetPosition(), diggingComponent->GetPosition());
    float size = TILE_SIZE * cameraComponent->GetZoom();

    const auto maxIndex = static_cast<float>(textureList.size() - 1);
    uint8_t crackIndex = static_cast<uint8_t>(std::min(diggingComponent->GetProgress() * maxIndex, maxIndex));
    SDL_FRect dstRect = {cameraVector2d.x - size / 2, cameraVector2d.y - size / 2, size, size};
    auto &crackTexture = textureList[crackIndex];
    if (crackTexture) {
        SDL_RenderTexture(renderer, crackTexture.get(), nullptr, &dstRect);
    }
}

uint8_t glimmer::DiggingSystem::GetRenderOrder() {
    return RENDER_ORDER_DIGGING;
}

std::string glimmer::DiggingSystem::GetName() {
    return "glimmer.DiggingSystem";
}
