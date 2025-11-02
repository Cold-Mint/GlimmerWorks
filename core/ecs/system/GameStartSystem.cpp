//
// Created by Cold-Mint on 2025/10/28.
//

#include "GameStartSystem.h"

#include "../component/CameraComponent.h"
#include "../component/WorldPositionComponent.h"
#include "../component/DebugDrawComponent.h"
#include "../component/PlayerControlComponent.h"


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
    worldPositionComponent->SetPosition(Vector2D(0, 0));
    auto debugDrawComponent = worldContext_->AddComponent<DebugDrawComponent>(playerEntity);
    debugDrawComponent->SetSize(Vector2D(100.0f, 100.0f));
    debugDrawComponent->SetColor(SDL_Color{255, 0, 0, 255});
    auto cameraComponent = worldContext_->AddComponent<CameraComponent>(playerEntity);
    cameraComponent->SetSpeed(1.0F);
    worldContext_->SetCameraComponent(cameraComponent);
    worldContext_->SetCameraPosition(worldPositionComponent);

    //在世界坐标15,5,放置一个蓝色debugDraw实体，尺寸是100*100
    auto blue = worldContext_->CreateEntity();
    auto bluedebug = worldContext_->AddComponent<DebugDrawComponent>(blue);
    bluedebug->SetColor(SDL_Color{0, 0, 255, 255});
    bluedebug->SetSize(Vector2D(100.0f, 100.0f));
    auto t = worldContext_->AddComponent<WorldPositionComponent>(blue);
    t->SetPosition(Vector2D(15, 5));
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
