//
// Created by Cold-Mint on 2025/10/28.
//

#include "GameStartSystem.h"

#include <random>

#include "../../log/LogCat.h"
#include "../component/TileLayerComponent.h"
#include "../../world/WorldContext.h"
#include "core/ecs/component/AreaMarkerComponent.h"
#include "core/ecs/component/ItemContainerComonent.h"


glimmer::GameStartSystem::GameStartSystem(WorldContext *worldContext) : GameSystem(worldContext) {
}

void glimmer::GameStartSystem::Update(float delta) {
    LogCat::d("Game Start System init");
    LogCat::i("Grid entity created with GridComponent");
    auto pause = worldContext_->CreateEntity();
    worldContext_->AddComponent<PauseComponent>(pause);

    auto groundTileLayerEntity = worldContext_->CreateEntity();
    worldContext_->AddComponent<
        TileLayerComponent>(groundTileLayerEntity, worldContext_, Ground);
    worldContext_->AddComponent<AreaMarkerComponent>(groundTileLayerEntity);
    auto backgroundTileLayerEntity = worldContext_->CreateEntity();
    worldContext_->AddComponent<
        TileLayerComponent>(backgroundTileLayerEntity, worldContext_, BackGround);
    auto appContext = worldContext_->GetAppContext();
    ResourceRef playerResourceRef{};
    playerResourceRef.ReadResource(*appContext->GetMobManager()->GetPlayerResourceList()[0],
                                   RESOURCE_TYPE_MOB);
    worldContext_->InitPlayer(
        playerResourceRef);
    worldContext_->InitHotbar(
        worldContext_->GetComponent<ItemContainerComponent>(worldContext_->GetPlayerEntity())->
        GetItemContainer());
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
