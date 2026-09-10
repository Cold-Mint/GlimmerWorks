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
#include "WorldContext.h"

#include "WorldBuilder.h"
#include "Dimension.h"
#include "ChunkManager.h"
#include "TerrainManager.h"
#include "SystemScheduler.h"
#include "PlayerContext.h"
#include "TileInstancePool.h"
#include "box2d/box2d.h"
#include "core/config/Constants.h"
#include "core/log/LogCat.h"
#include "core/context/AppContext.h"


bool glimmer::WorldContext::IsDragMode() const {
    return dragMode_;
}

void glimmer::WorldContext::SetDragMode(const bool dragMode) {
    dragMode_ = dragMode;
}

glimmer::EntityManager *glimmer::WorldContext::GetEntityManager() const {
    if (entityManager_ == nullptr) {
        LogCat::w(std::source_location::current(), "entity_manager_is_null", "entityManager is nullptr");
        return nullptr;
    }
    return entityManager_.get();
}

glimmer::EntityShortCut *glimmer::WorldContext::GetEntityShortCut() const {
    return entityShortCut_.get();
}

bool glimmer::WorldContext::IsRuning() const {
    return running;
}

void glimmer::WorldContext::SetRuning(const bool run) {
    running = run;
}

glimmer::Saves *glimmer::WorldContext::GetSaves() const {
    if (saves_ == nullptr) {
        LogCat::w(std::source_location::current(), "saves_is_null", "saves is nullptr");
        return nullptr;
    }
    return saves_;
}

glimmer::MapManifest *glimmer::WorldContext::GetMapManifest() const {
    return mapManifest_.get();
}

glimmer::ChunkGenerator *glimmer::WorldContext::GetChunkGenerator() const {
    return chunkGenerator_.get();
}

glimmer::ChunkLoader *glimmer::WorldContext::GetChunkLoader() const {
    return chunkLoader_.get();
}

glimmer::AppContext *glimmer::WorldContext::GetAppContext() const {
    return appContext_;
}

b2WorldId glimmer::WorldContext::GetWorldId() const {
    return worldId_;
}

int glimmer::WorldContext::GetWorldSeed() const {
    return worldSeed_;
}

bool glimmer::WorldContext::IsEmptyEntityId(const uint32_t id) {
    return id == GAME_ENTITY_ID_INVALID;
}

glimmer::ChunkManager *glimmer::WorldContext::GetChunkManager() const {
    return chunkManager_.get();
}

glimmer::TerrainManager *glimmer::WorldContext::GetTerrainManager() const {
    return terrainManager_.get();
}

glimmer::SystemScheduler *glimmer::WorldContext::GetSystemScheduler() const {
    return systemScheduler_.get();
}

glimmer::PlayerContext *glimmer::WorldContext::GetPlayerContext() const {
    return playerContext_.get();
}

void glimmer::WorldContext::SaveEntity(EntityItemMessage *entityItemMessage, const GameEntityID entityId) const {
    worldSaver_.SaveEntity(entityItemMessage, entityId);
}

void glimmer::WorldContext::SaveGame() {
    worldSaver_.SaveGame();
}

glimmer::LightBuffer *glimmer::WorldContext::GetLightingBuffer() const {
    LightBuffer *result = chunkManager_->GetLightingBuffer();
    if (result == nullptr) {
        LogCat::w(std::source_location::current(), "light_buffer_is_null", "light buffer is null");
    }
    return result;
}

glimmer::TileInstancePool *glimmer::WorldContext::GetTileInstancePool() const {
    return tileInstancePool_.get();
}

glimmer::Dimension *glimmer::WorldContext::GetDimension() const {
    return dimension_.get();
}

glimmer::WorldContext::~WorldContext() {
    LogCat::i("world_context_destroying", "Destroying WorldContext: worldName={}",
              mapManifest_ ? mapManifest_->name : "unknown");
    playerContext_.reset();
    systemScheduler_.reset();
    LogCat::d("world_context_player_scheduler_released", "PlayerContext and SystemScheduler released");
    if (entityManager_) {
        entityManager_->Clear();
    }
    LogCat::d("world_context_entity_manager_cleared", "EntityManager cleared, dimensions released");
    b2DestroyWorld(worldId_);
    worldId_ = b2_nullWorldId;
    if (appContext_) {
        appContext_->GetConsoleContext()->GetCommandManager()->UnbindWorldContext();
    }
    LogCat::i("world_context_destroyed", "WorldContext destroyed");
}

void glimmer::WorldContext::OnTick(const uint64_t tick) {
    if (!initedTick_) {
        startTick_ = tick;
        initedTick_ = true;
    }
    lastTick_ = tick;
}

uint64_t glimmer::WorldContext::GetGlobalTick() const {
    return fixedGlobalTick_ + (lastTick_ - startTick_);
}

glimmer::WorldContext::WorldContext(AppContext *appContext, Saves *saves) : saves_(saves),
                                                                            appContext_(appContext),
                                                                            worldSaver_(this) {
    WorldBuilder builder(this);
    builder.Build();
}
