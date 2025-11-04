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
    worldPositionComponent->SetPosition(Vector2D(0, 0));
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
    auto tileLayerComponent = worldContext_->AddComponent<TileLayerComponent>(tileLayer);
    const int MAP_WIDTH = 1000;
    const int MAP_HEIGHT = 1000;

    // 随机数生成器
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> colorDist(0, 2); // 0=石头, 1=泥土, 2=草地

    Tile tile;
    tile.color = {100, 100, 100, 255};
    tileLayerComponent->SetTile({-2, -2}, tile);


    for (int y = 0; y < MAP_HEIGHT; ++y)
    {
        for (int x = 0; x < MAP_WIDTH; ++x)
        {
            Tile tile;

            // 随机选择颜色类型
            switch (colorDist(rng))
            {
            case 0: // 石头（灰）
                tile.color = {100, 100, 100, 255};
                break;
            case 1: // 泥土（棕）
                tile.color = {139, 69, 19, 255};
                break;
            case 2: // 草地（绿）
                tile.color = {34, 139, 34, 255};
                break;
            }

            tileLayerComponent->SetTile({x, y}, tile);
        }
    }
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
