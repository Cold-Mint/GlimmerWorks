//
// Created by Cold-Mint on 2025/10/28.
//

#include "GameStartSystem.h"

#include <random>

#include "../../log/LogCat.h"
#include "../component/TileLayerComponent.h"
#include "../../world/WorldContext.h"


void glimmer::GameStartSystem::Update(float delta) {
    LogCat::d("Game Start System init");
    LogCat::i("Grid entity created with GridComponent");
    auto pause = worldContext_->CreateEntity();
    worldContext_->AddComponent<PauseComponent>(pause->GetID());

    auto *tileLayerEntity = worldContext_->CreateEntity();
    auto tileId = tileLayerEntity->GetID();
    worldContext_->AddComponent<
        TileLayerComponent>(tileId, TileLayerType::Main, worldContext_->GetAllChunks());
    worldContext_->InitPlayer();
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
