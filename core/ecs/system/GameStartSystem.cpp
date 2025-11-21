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


    const auto tileLayerEntity = worldContext_->CreateEntity();
    const auto transform2DComponent = worldContext_->AddComponent<Transform2DComponent>(tileLayerEntity);
    transform2DComponent->SetPosition(Vector2D(0, 0));
    worldContext_->AddComponent<TileLayerComponent>(tileLayerEntity);


    const auto playerEntity = worldContext_->CreateEntity();
    worldContext_->AddComponent<PlayerControlComponent>(playerEntity);
    const auto transform2DComponentInPlayer = worldContext_->AddComponent<Transform2DComponent>(playerEntity);
    const auto heightMap = worldContext_->GetHeightMap(0);
    transform2DComponentInPlayer->SetPosition(
        TileLayerComponent::TileToWorld(transform2DComponentInPlayer->GetPosition(),
                                        TileVector2D(0, heightMap[0] + 3)));
    const auto debugDrawComponent = worldContext_->AddComponent<DebugDrawComponent>(playerEntity);
    debugDrawComponent->SetSize(Vector2D(static_cast<float>(TILE_SIZE), static_cast<float>(TILE_SIZE)));
    debugDrawComponent->SetColor(SDL_Color{255, 0, 0, 255});
    const auto cameraComponent = worldContext_->AddComponent<CameraComponent>(playerEntity);
    cameraComponent->SetSpeed(1.0F);
    worldContext_->SetCameraComponent(cameraComponent);
    worldContext_->SetCameraPosition(transform2DComponentInPlayer);
    const auto rigidBody2DComponent = worldContext_->AddComponent<RigidBody2DComponent>(playerEntity);
    rigidBody2DComponent->SetBodyType(b2_dynamicBody);
    rigidBody2DComponent->SetEnableSleep(false);
    rigidBody2DComponent->SetFixedRotation(true);
    rigidBody2DComponent->SetWidth(1.25F * TILE_SIZE);
    rigidBody2DComponent->SetHeight(2.6F * TILE_SIZE);
    rigidBody2DComponent->CreateBody(worldContext_->GetWorldId(),
                                     Box2DUtils::ToMeters(transform2DComponentInPlayer->GetPosition()));
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
