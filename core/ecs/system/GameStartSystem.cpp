//
// Created by Cold-Mint on 2025/10/28.
//

#include "GameStartSystem.h"

#include <random>

#include "../../Constants.h"
#include "../../log/LogCat.h"
#include "../component/CameraComponent.h"
#include "../component/WorldPositionComponent.h"
#include "../component/DebugDrawComponent.h"
#include "../component/PlayerControlComponent.h"
#include "../component/TileLayerComponent.h"
#include "../../world/WorldContext.h"


namespace glimmer
{
    class DebugDrawComponent;
}

void glimmer::GameStartSystem::Update(float delta)
{
    LogCat::d("Game Start System init");
    LogCat::i("Grid entity created with GridComponent");
    auto playerEntity = worldContext_->CreateEntity();
    // 添加玩家控制组件
    auto playerControl = worldContext_->AddComponent<PlayerControlComponent>(playerEntity);
    playerControl->enableWASD = true;
    auto worldPositionComponent = worldContext_->AddComponent<WorldPositionComponent>(playerEntity);
    worldPositionComponent->SetPosition(Vector2D(0, 256 * CHUNK_SIZE));
    auto debugDrawComponent = worldContext_->AddComponent<DebugDrawComponent>(playerEntity);
    debugDrawComponent->SetSize(Vector2D(static_cast<float>(TILE_SIZE), static_cast<float>(TILE_SIZE)));
    debugDrawComponent->SetColor(SDL_Color{255, 0, 0, 255});
    auto cameraComponent = worldContext_->AddComponent<CameraComponent>(playerEntity);
    cameraComponent->SetSpeed(1.0F);
    worldContext_->SetCameraComponent(cameraComponent);
    worldContext_->SetCameraPosition(worldPositionComponent);

    auto tileLayer = worldContext_->CreateEntity();
    auto t1 = worldContext_->AddComponent<WorldPositionComponent>(tileLayer);
    t1->SetPosition(Vector2D(0, 0));
    worldContext_->AddComponent<TileLayerComponent>(tileLayer);
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
