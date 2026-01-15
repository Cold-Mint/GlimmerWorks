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
#include "core/ecs/component/GuiTransform2DComponent.h"


void glimmer::GameStartSystem::Update(float delta) {
    LogCat::d("Game Start System init");
    LogCat::i("Grid entity created with GridComponent");
    auto pause = worldContext_->CreateEntity();
    worldContext_->AddComponent<PauseComponent>(pause->GetID());

    auto *tileLayerEntity = worldContext_->CreateEntity();
    auto tileId = tileLayerEntity->GetID();
    worldContext_->AddComponent<
        TileLayerComponent>(tileId, TileLayerType::Main, worldContext_->GetAllChunks());
    auto *playerEntity = worldContext_->CreateEntity();
    auto playerId = playerEntity->GetID();
    worldContext_->SetPlayerEntity(playerEntity);
    worldContext_->AddComponent<PlayerControlComponent>(playerId);
    const auto transform2DComponentInPlayer = worldContext_->AddComponent<Transform2DComponent>(playerId);
    const auto height = worldContext_->GetHeight(0);
    transform2DComponentInPlayer->SetPosition(
        TileLayerComponent::TileToWorld(TileVector2D(0, height + 3)));
    const auto debugDrawComponent = worldContext_->AddComponent<DebugDrawComponent>(playerId);
    debugDrawComponent->SetSize(Vector2D(static_cast<float>(TILE_SIZE), static_cast<float>(TILE_SIZE)));
    debugDrawComponent->SetColor(SDL_Color{255, 0, 0, 255});
    const auto cameraComponent = worldContext_->AddComponent<CameraComponent>(playerId);
    worldContext_->SetCameraComponent(cameraComponent);
    const auto diggingComponent = worldContext_->AddComponent<DiggingComponent>(playerId);
    worldContext_->SetDiggingComponent(diggingComponent);
    worldContext_->SetCameraPosition(transform2DComponentInPlayer);
    const auto rigidBody2DComponent = worldContext_->AddComponent<RigidBody2DComponent>(playerId);
    rigidBody2DComponent->SetBodyType(b2_dynamicBody);
    rigidBody2DComponent->SetEnableSleep(false);
    rigidBody2DComponent->SetFixedRotation(true);
    rigidBody2DComponent->SetWidth(1.25F * TILE_SIZE);
    rigidBody2DComponent->SetHeight(2.6F * TILE_SIZE);
    rigidBody2DComponent->SetCategoryBits(BOX2D_CATEGORY_PLAYER);
    rigidBody2DComponent->SetMaskBits(BOX2D_CATEGORY_TILE);
    rigidBody2DComponent->CreateBody(worldContext_->GetWorldId(),
                                     Box2DUtils::ToMeters(transform2DComponentInPlayer->GetPosition()));
    auto *magnetComponent = worldContext_->AddComponent<MagnetComponent>(playerId);
    magnetComponent->SetType(MAGNETIC_TYPE_ITEM);
    worldContext_->AddComponent<ItemContainerComponent>(playerId, 9);
    worldContext_->AddComponent<AutoPickComponent>(playerId);
    const auto hotBarComponent = worldContext_->AddComponent<HotBarComponent>(playerId, Vector2D(), 9);
    worldContext_->SetHotBarComponent(hotBarComponent);

    for (int i = 0; i < 9; ++i) {
        auto *slotEntity = worldContext_->CreateEntity();
        auto slotId = slotEntity->GetID();
        worldContext_->AddComponent<ItemSlotComponent>(slotId, playerEntity, i);
        worldContext_->AddComponent<GuiTransform2DComponent>(slotId);
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
