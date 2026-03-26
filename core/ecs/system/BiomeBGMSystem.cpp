//
// Created by coldmint on 2026/3/26.
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
}

void glimmer::BiomeBGMSystem::Update(float delta) {
    if (worldContext_ == nullptr) {
        return;
    }
    AppContext *appContext = worldContext_->GetAppContext();
    if (appContext == nullptr) {
        return;
    }
    AudioManager *audioManager = appContext->GetAudioManager();
    if (audioManager == nullptr) {
        return;
    }
    ResourceLocator *resourceLocator = appContext->GetResourceLocator();
    if (resourceLocator == nullptr) {
        return;
    }
    GameEntity::ID player = worldContext_->GetPlayerEntity();
    if (WorldContext::IsEmptyEntityId(player)) {
        return;
    }
    auto transform2D = worldContext_->GetComponent<Transform2DComponent>(player);
    if (transform2D == nullptr) {
        return;
    }
    const WorldVector2D position = transform2D->GetPosition();
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
    std::shared_ptr<MIX_Audio> audio = resourceLocator->FindAudio(biomeResource->bgm);
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
