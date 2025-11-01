//
// Created by Cold-Mint on 2025/10/28.
//

#include "GameStartSystem.h"

#include "../component/CameraComponent.h"
#include "../component/WorldPositionComponent.h"
#include "../component/GridComponent.h"
#include "../component/DebugDrawComponent.h"
#include "../component/PlayerControlComponent.h"


namespace glimmer
{
    class DebugDrawComponent;
}

void glimmer::GameStartSystem::Update(float delta)
{
    LogCat::d("Game Start System init");
    // 创建一个实体用于展示网格
    auto gridEntity = worldContext_->CreateEntity();
    // 添加网格组件
    auto gridComponent = worldContext_->AddComponent<GridComponent>(gridEntity);
    // 配置网格属性
    gridComponent->startPoint = Vector2D(0, 0);
    gridComponent->width = 100;
    gridComponent->height = 100;
    gridComponent->gridSizeX = 50;
    gridComponent->gridSizeY = 50;
    gridComponent->r = 0;
    gridComponent->g = 0;
    gridComponent->b = 255;
    gridComponent->a = 255;
    gridComponent->lineWidth = 1;

    LogCat::i("Grid entity created with GridComponent");

    // // 创建相机实体
    // auto cameraEntity = worldContext->CreateEntity();
    // // 添加相机组件
    // auto cameraComponent = worldContext->AddComponent<CameraComponent>(cameraEntity);
    // cameraComponent->SetSpeed(200.0f); // 相机移动速度
    // cameraComponent->SetZoom(1.0f); // 默认缩放比例
    auto playerEntity = worldContext_->CreateEntity();
    // 添加玩家控制组件
    auto playerControl = worldContext_->AddComponent<PlayerControlComponent>(playerEntity);
    playerControl->enableWASD = true;
    auto worldPositionComponent = worldContext_->AddComponent<WorldPositionComponent>(playerEntity);
    worldPositionComponent->SetPosition(Vector2D(200, 150));
    auto debugDrawComponent = worldContext_->AddComponent<DebugDrawComponent>(playerEntity);
    debugDrawComponent->SetSize(Vector2D(100.0f, 100.0f));
    debugDrawComponent->SetColor(SDL_Color{255, 0, 0, 255});
    auto cameraComponent = worldContext_->AddComponent<CameraComponent>(playerEntity);
    cameraComponent->SetSpeed(1.0F);
    worldContext_->SetCameraComponent(cameraComponent);
    worldContext_->SetCameraPosition(worldPositionComponent);
    // cameraComponent
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
