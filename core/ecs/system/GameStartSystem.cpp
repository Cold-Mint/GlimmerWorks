//
// Created by Cold-Mint on 2025/10/28.
//

#include "GameStartSystem.h"

#include <random>

#include "../../Constants.h"
#include "../../log/LogCat.h"
#include "../../utils/Box2DUtils.h"
#include "../component/CameraComponent.h"
#include "../component/Transform2DComponent.h"
#include "../component/DebugDrawComponent.h"
#include "../component/PlayerControlComponent.h"
#include "../component/TileLayerComponent.h"
#include "../component/RigidBody2DComponent.h"
#include "../../world/WorldContext.h"
#include "../component/HotBarComonent.h"
#include "../component/ItemContainerComonent.h"
#include "../component/MagnetComponent.h"
#include "../component/AutoPickComponent.h"
#include "../component/ItemSlotComponent.h"


void glimmer::GameStartSystem::Update(float delta) {
    LogCat::d("Game Start System init");
    LogCat::i("Grid entity created with GridComponent");


    auto *tileLayerEntity = worldContext_->CreateEntity();
    const auto transform2DComponent = worldContext_->AddComponent<Transform2DComponent>(tileLayerEntity);
    transform2DComponent->SetPosition(Vector2D(0, 0));
    worldContext_->AddComponent<
        TileLayerComponent>(tileLayerEntity, TileLayerType::Main, worldContext_->GetAllChunks());
    auto *playerEntity = worldContext_->CreateEntity();
    worldContext_->SetPlayerEntity(playerEntity);
    worldContext_->AddComponent<PlayerControlComponent>(playerEntity);
    const auto transform2DComponentInPlayer = worldContext_->AddComponent<Transform2DComponent>(playerEntity);
    const auto height = worldContext_->GetHeight(0);
    transform2DComponentInPlayer->SetPosition(
        TileLayerComponent::TileToWorld(transform2DComponentInPlayer->GetPosition(),
                                        TileVector2D(0, height + 3)));
    const auto debugDrawComponent = worldContext_->AddComponent<DebugDrawComponent>(playerEntity);
    debugDrawComponent->SetSize(Vector2D(static_cast<float>(TILE_SIZE), static_cast<float>(TILE_SIZE)));
    debugDrawComponent->SetColor(SDL_Color{255, 0, 0, 255});
    const auto cameraComponent = worldContext_->AddComponent<CameraComponent>(playerEntity);
    worldContext_->SetCameraComponent(cameraComponent);
    const auto diggingComponent = worldContext_->AddComponent<DiggingComponent>(playerEntity);
    worldContext_->SetDiggingComponent(diggingComponent);
    worldContext_->SetCameraPosition(transform2DComponentInPlayer);
    const auto rigidBody2DComponent = worldContext_->AddComponent<RigidBody2DComponent>(playerEntity);
    rigidBody2DComponent->SetBodyType(b2_dynamicBody);
    rigidBody2DComponent->SetEnableSleep(false);
    rigidBody2DComponent->SetFixedRotation(true);
    rigidBody2DComponent->SetWidth(1.25F * TILE_SIZE);
    rigidBody2DComponent->SetHeight(2.6F * TILE_SIZE);
    rigidBody2DComponent->SetCategoryBits(BOX2D_CATEGORY_PLAYER);
    rigidBody2DComponent->SetMaskBits(BOX2D_CATEGORY_TILE);
    rigidBody2DComponent->CreateBody(worldContext_->GetWorldId(),
                                     Box2DUtils::ToMeters(transform2DComponentInPlayer->GetPosition()));
    auto *magnetComponent = worldContext_->AddComponent<MagnetComponent>(playerEntity);
    magnetComponent->SetType(MAGNETIC_TYPE_ITEM);
    worldContext_->AddComponent<ItemContainerComponent>(playerEntity, 9);
    worldContext_->AddComponent<AutoPickComponent>(playerEntity);
    const auto hotBarComponent = worldContext_->AddComponent<HotBarComponent>(playerEntity, Vector2D(), 9);
    worldContext_->SetHotBarComponent(hotBarComponent);

    for (int i = 0; i < 9; ++i) {
        auto *slotEntity = worldContext_->CreateEntity();
        worldContext_->AddComponent<ItemSlotComponent>(slotEntity, playerEntity, i);
        worldContext_->AddComponent<Transform2DComponent>(slotEntity); // Position managed by HotBarSystem
        hotBarComponent->AddSlotEntity(slotEntity);
    }
    LogCat::i("Camera entity created with CameraComponent, WorldPositionComponent and PlayerControlComponent");
}

std::string glimmer::GameStartSystem::GetName() {
    return "glimmer.GameStartSystem";
}

bool glimmer::GameStartSystem::ShouldActivate() {
    if (shouldStart) {
        shouldStart = false;
        return true;
    }
    return false;
}
