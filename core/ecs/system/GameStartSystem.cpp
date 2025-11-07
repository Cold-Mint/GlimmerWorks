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


void glimmer::GameStartSystem::Update(float delta)
{
    LogCat::d("Game Start System init");
    LogCat::i("Grid entity created with GridComponent");


    auto tileLayerEntity = worldContext_->CreateEntity();
    auto transform2DComponent = worldContext_->AddComponent<Transform2DComponent>(tileLayerEntity);
    transform2DComponent->SetPosition(Vector2D(0, 0));
    worldContext_->AddComponent<TileLayerComponent>(tileLayerEntity);


    auto playerEntity = worldContext_->CreateEntity();
    // 添加玩家控制组件
    auto playerControl = worldContext_->AddComponent<PlayerControlComponent>(playerEntity);
    playerControl->enableWASD = true;
    auto worldPositionComponent = worldContext_->AddComponent<Transform2DComponent>(playerEntity);
    worldPositionComponent->SetPosition(
        TileLayerComponent::TileToWorld(transform2DComponent->GetPosition(), TileVector2D(0, 100)));
    auto debugDrawComponent = worldContext_->AddComponent<DebugDrawComponent>(playerEntity);
    debugDrawComponent->SetSize(Vector2D(static_cast<float>(TILE_SIZE), static_cast<float>(TILE_SIZE)));
    debugDrawComponent->SetColor(SDL_Color{255, 0, 0, 255});
    auto cameraComponent = worldContext_->AddComponent<CameraComponent>(playerEntity);
    cameraComponent->SetSpeed(1.0F);
    worldContext_->SetCameraComponent(cameraComponent);
    worldContext_->SetCameraPosition(worldPositionComponent);
    const auto rigidBody2DComponent = worldContext_->AddComponent<RigidBody2DComponent>(playerEntity);
    rigidBody2DComponent->SetBodyType(b2_dynamicBody);

    rigidBody2DComponent->CreateBody(worldContext_->GetWorldId(),
                                     Box2DUtils::ToMeters(worldPositionComponent->GetPosition()),
                                     b2MakeBox(Box2DUtils::ToMeters(TILE_SIZE), Box2DUtils::ToMeters(TILE_SIZE)));
    LogCat::i("Camera entity created with CameraComponent, WorldPositionComponent and PlayerControlComponent");
}

std::string glimmer::GameStartSystem::GetName()
{
    return "glimmer.GameStartSystem";
}

bool glimmer::GameStartSystem::ShouldActivate()
{
    if (shouldStart)
    {
        shouldStart = false;
        return true;
    }
    return false;
}
