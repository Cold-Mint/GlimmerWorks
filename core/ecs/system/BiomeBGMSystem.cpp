//
// Created by Cold-Mint on 2026/3/26.
//

#include "BiomeBGMSystem.h"

#include "core/ecs/component/PlayerComponent.h"
#include "core/ecs/component/TileLayerComponent.h"
#include "core/ecs/component/Transform2DComponent.h"
#include "core/world/WorldContext.h"

glimmer::BiomeBGMSystem::BiomeBGMSystem(WorldContext *worldContext) : GameSystem(worldContext) {
    RequireComponent<PlayerComponent>();
    RequireComponent<TileLayerComponent>();
    RequireComponent<Transform2DComponent>();
    GameEntity::ID player = worldContext_->GetPlayerEntity();
    if (WorldContext::IsEmptyEntityId(player)) {
        return;
    }
    appContext_ = worldContext->GetAppContext();
    playerTransform2DComponent_ = worldContext_->GetComponent<Transform2DComponent>(player);
}

void glimmer::BiomeBGMSystem::Update(float delta) {
    if (worldContext_ == nullptr) {
        return;
    }
    if (appContext_ == nullptr) {
        return;
    }
    AudioManager *audioManager = appContext_->GetAudioManager();
    if (audioManager == nullptr) {
        return;
    }
    ResourceLocator *resourceLocator = appContext_->GetResourceLocator();
    if (resourceLocator == nullptr) {
        return;
    }
    if (playerTransform2DComponent_ == nullptr) {
        return;
    }
    const WorldVector2D position = playerTransform2DComponent_->GetPosition();
    const TileVector2D tileVector2d = TileLayerComponent::WorldToTile(position);
    const TileVector2D chunkVertex = Chunk::TileCoordinatesToChunkVertexCoordinates(tileVector2d);
    const TerrainResult *terrainResult = worldContext_->GetTerrainData(chunkVertex);
    if (terrainResult == nullptr) {
        return;
    }
    TileVector2D chunkRelative = Chunk::TileCoordinatesToChunkRelativeCoordinates(tileVector2d);
    const TerrainTileResult &terrainTileResult = terrainResult->QueryTerrain(chunkRelative.x, chunkRelative.y);
    BiomeResource *biomeResource = terrainTileResult.biomeResource;
    if (biomeResource == nullptr || biomeResource == biomeResource_) {
        return;
    }
    std::shared_ptr<MIX_Audio> audio = resourceLocator->FindAudio(&biomeResource->bgm);
    if (audio == nullptr) {
        return;
    }
    audio_ = audio;
    audioManager->ForcePlayReplace(BGM, audio_.get(), -1);
    biomeResource_ = biomeResource;
}


std::string glimmer::BiomeBGMSystem::GetName() {
    return "glimmer.BiomeBGMSystem";
}
