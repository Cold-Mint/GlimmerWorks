/*
 * Copyright (C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * 版权(C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * 本程序是自由软件：你可以遵照自由软件基金会出版的GNU Affero通用公共许可证条款来重新分发和修改它
 * 该许可证的第3版，或者（由你选择）任何后续版本。
 *
 * 本程序的发布目的是希望它能有用，但没有任何担保；甚至没有适销性或特定用途适用性的默示担保。
 * 有关详细细节，请参阅GNU Affero通用公共许可证。
 *
 * 你应该已经收到一份GNU Affero通用公共许可证的副本。如果没有，请查阅<https://www.gnu.org/licenses/>。
 */
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
