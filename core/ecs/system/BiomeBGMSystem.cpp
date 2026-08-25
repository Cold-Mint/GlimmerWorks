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

#include "core/ecs/component/Transform2DComponent.h"
#include "core/math/CoordinateTransformer.h"
#include "core/mod/resourcePack/AudioResourceResult.h"
#include "core/world/WorldContext.h"
#include "core/world/TerrainManager.h"

void glimmer::BiomeBGMSystem::OnWatchedComponentChanged(GameComponentTypeMessage gameComponentType, uint32_t count) {
    const EntityShortCut *entityShortCut = GetEntityShortCut();
    EntityManager *entityManager = GetEntityManager();
    if (gameComponentType == COMPONENT_TRANSFORM_2D && playerTransform2DComponent_ == nullptr) {
        GameEntityID player = entityShortCut->GetPlayer();
        if (!WorldContext::IsEmptyEntityId(player)) {
            playerTransform2DComponent_ = entityManager->GetComponent<Transform2DComponent>(player);
        }
    }
}

void glimmer::BiomeBGMSystem::SwitchToBiome(BiomeResource *biomeResource) {
    std::shared_ptr<AudioResourceResult> audioResourceResult = resourceLocator_->FindAudio(&biomeResource->bgm);
    if (audioResourceResult == nullptr) {
        return;
    }
    audioResult_ = audioResourceResult;
    if (MIX_Audio *audio = audioResult_->GetResource(); audio != nullptr) {
        audioManager_->ForcePlayReplace(AudioType::BGM, audio, -1);
    }
    biomeResource_ = biomeResource;
}

glimmer::BiomeBGMSystem::BiomeBGMSystem(WorldContext *worldContext) : GameSystem(worldContext) {
    WatchComponent(COMPONENT_TRANSFORM_2D);
    AppContext *appContext = worldContext->GetAppContext();
    if (appContext != nullptr) {
        audioManager_ = appContext->GetAudioContext()->GetAudioManager();
        resourceLocator_ = appContext->GetResourceLocator();
    }
    Init();
}

void glimmer::BiomeBGMSystem::Update(float delta) {
    WorldContext *worldContext = GetWorldContext();
    if (worldContext == nullptr) {
        return;
    }
    if (playerTransform2DComponent_ == nullptr) {
        return;
    }
    if (resourceLocator_ == nullptr) {
        return;
    }
    if (audioManager_ == nullptr) {
        return;
    }
    TerrainManager *terrainManager = worldContext->GetTerrainManager();
    if (terrainManager == nullptr) {
        return;
    }
    const WorldVector2D position = playerTransform2DComponent_->GetPosition();
    const TileVector2D tileVector2d = CoordinateTransformer::WorldToTile(position);
    const TileVector2D chunkVertex = Chunk::TileCoordinatesToChunkVertexCoordinates(tileVector2d);

    const TerrainResult *terrainResult = terrainManager->GetTerrainData(chunkVertex);
    if (terrainResult == nullptr) {
        return;
    }
    TileVector2D chunkRelative = Chunk::TileCoordinatesToChunkRelativeCoordinates(tileVector2d);
    const TerrainTileResult &terrainTileResult = terrainResult->QueryTerrain(chunkRelative.x, chunkRelative.y);
    BiomeResource *biomeResource = terrainTileResult.biomeResource;
    if (biomeResource == nullptr) {
        return;
    }

    // Player is back in the biome whose BGM is already playing: cancel any pending switch.
    // 玩家回到了正在播放 BGM 的生物群系：取消待切换。
    if (biomeResource == biomeResource_) {
        candidateBiomeResource_ = nullptr;
        candidateTimeAccumulator_ = 0.0F;
        return;
    }

    // No BGM playing yet (initial spawn): switch immediately without debouncing.
    // 尚未播放任何 BGM（初次进入）：立即切换，不做防抖。
    if (biomeResource_ == nullptr) {
        SwitchToBiome(biomeResource);
        return;
    }

    // Debounce: only switch after the player stays in the new biome for a while,
    // so brief crossings (e.g. a single jump) don't trigger BGM churn.
    // 防抖：只有当玩家在新生物群系中连续停留一段时间后才切换，
    // 避免短暂跨越（如单次跳跃）导致 BGM 频繁切换。
    if (biomeResource != candidateBiomeResource_) {
        candidateBiomeResource_ = biomeResource;
        candidateTimeAccumulator_ = 0.0F;
    }
    candidateTimeAccumulator_ += delta;

    const AppContext *appContext = worldContext->GetAppContext();
    const Config *config = appContext != nullptr ? appContext->GetConfig() : nullptr;
    const float debounceSeconds = config != nullptr
                                      ? config->biomeBgm.debounceSeconds
                                      : kDefaultBiomeBGMDebounceSeconds;
    if (candidateTimeAccumulator_ < debounceSeconds) {
        return;
    }

    SwitchToBiome(biomeResource);
    candidateBiomeResource_ = nullptr;
    candidateTimeAccumulator_ = 0.0F;
}

glimmer::GameSystemType glimmer::BiomeBGMSystem::GetGameSystemType() const {
    return GameSystemType::BiomeBGMSystem;
}
