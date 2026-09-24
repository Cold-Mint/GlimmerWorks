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
#include "TechProviderSystem.h"

#include <utility>

#include "core/context/AppContext.h"
#include "core/ecs/component/PlayerComponent.h"
#include "core/ecs/component/TechProviderComponent.h"
#include "core/log/LogCat.h"
#include "core/scene/MainThreadDispatcher.h"
#include "core/world/WorldContext.h"

glimmer::TechProviderSystem::TechProviderSystem(WorldContext *worldContext)
    : GameSystem(worldContext) {
    WatchComponent(COMPONENT_TRANSFORM_2D);
    WatchComponent(COMPONENT_TECH_PROVIDER);
    WatchComponent(COMPONENT_PLAYER);
    Init();
}

void glimmer::TechProviderSystem::OnActivationChanged(bool activeStatus) {
    EntityManager *entityManager = GetEntityManager();
    if (!activeStatus) {
        if (WorldContext::IsEmptyEntityId(player_)) {
            return;
        }
        auto playerComponent = entityManager->GetComponent<PlayerComponent>(player_);
        if (playerComponent == nullptr) {
            return;
        }
        PlayerTechnologyHandler *playerTechnologyHandler = playerComponent->GetTechnologyHandler();
        if (playerTechnologyHandler == nullptr) {
            return;
        }
        playerTechnologyHandler->ResetTechnologyMap();
        LogCat::d("tech_provider_reset_on_deactivate",
                  "TechProvider reset technology map on deactivate");
    }
}

void glimmer::TechProviderSystem::OnTick(const uint64_t tick) {
    EntityManager *entityManager = GetEntityManager();
    if (WorldContext::IsEmptyEntityId(player_)) {
        return;
    }
    auto playerTransform2DComponent = entityManager->GetComponent<Transform2DComponent>(player_);
    if (playerTransform2DComponent == nullptr) {
        return;
    }
    //Consume the "recompute" flag set by OnWatchedComponentChanged. If not
    //flagged, only recompute after the player has moved more than one tile.
    //消费 OnWatchedComponentChanged 设置的"重新计算"标记；若未标记，则仅在玩家移动超过一个图块后重新计算。
    if (!changed.exchange(false, std::memory_order_relaxed) &&
        playerTransform2DComponent->GetPosition().DistanceSquared(lastPlayerPosition_) <
        TILE_SIZE * TILE_SIZE) {
        return;
    }
    lastPlayerPosition_ = playerTransform2DComponent->GetPosition();
    auto playerComponent = entityManager->GetComponent<PlayerComponent>(player_);
    if (playerComponent == nullptr) {
        return;
    }
    PlayerTechnologyHandler *playerTechnologyHandler = playerComponent->GetTechnologyHandler();
    if (playerTechnologyHandler == nullptr) {
        return;
    }
    std::vector<GameEntityID> techProviderEntities;
    {
        std::lock_guard lock(techProviderMutex_);
        techProviderEntities = techProviderEntities_;
    }
    std::vector<std::pair<RecipeGroup, uint8_t> > technologies;
    for (GameEntityID techProviderEntity: techProviderEntities) {
        auto providerTransform2DComponent = entityManager->GetComponent<Transform2DComponent>(
            techProviderEntity);
        if (providerTransform2DComponent == nullptr) {
            continue;
        }
        if (playerTransform2DComponent->GetPosition().Distance(
                providerTransform2DComponent->GetPosition()) / TILE_SIZE > WORK_STATION_RANGE) {
            continue;
        }
        auto techProviderComponent = entityManager->GetComponent<TechProviderComponent>(
            techProviderEntity);
        if (techProviderComponent == nullptr) {
            continue;
        }
        technologies.emplace_back(techProviderComponent->GetRecipeGroup(),
                                  techProviderComponent->GetTechnologyLevel());
    }
    LogCat::d("tech_provider_technology_applied", "TechProvider applied technology from {} providers",
              technologies.size());

    //The technology map is read by GUI systems on the main thread, so apply the
    //recomputed map on the main thread.
    //科技表会被主线程的 GUI 系统读取，因此在主线程应用重新计算的科技表。
    WorldContext *worldContext = GetWorldContext();
    const AppContext *appContext = worldContext != nullptr ? worldContext->GetAppContext() : nullptr;
    MainThreadDispatcher *dispatcher = appContext != nullptr ? appContext->GetMainThreadDispatcher() : nullptr;
    if (dispatcher == nullptr) {
        return;
    }
    dispatcher->PostToNextMainFrame([playerTechnologyHandler, technologies] {
        playerTechnologyHandler->ResetTechnologyMap();
        for (const auto &[recipeGroup, technologyLevel]: technologies) {
            playerTechnologyHandler->SetTechnology(recipeGroup, technologyLevel);
        }
    });
}

void glimmer::TechProviderSystem::OnWatchedComponentChanged(GameComponentTypeMessage gameComponentType,
                                                            uint32_t count) {
    EntityShortCut *entityShortCut = GetEntityShortCut();
    EntityManager *entityManager = GetEntityManager();
    if (gameComponentType == COMPONENT_TRANSFORM_2D) {
        transform2DCount_ = count;
    }
    if (gameComponentType == COMPONENT_PLAYER) {
        player_ = entityShortCut->GetPlayer();
    }
    if (gameComponentType == COMPONENT_TECH_PROVIDER) {
        techProviderCount_ = count;
    }
    if (transform2DCount_ > 0 && techProviderCount_ > 0) {
        std::lock_guard lock(techProviderMutex_);
        techProviderEntities_.clear();
        techProviderEntities_ = entityManager->GetEntityIDWithComponents({
            COMPONENT_TRANSFORM_2D, COMPONENT_TECH_PROVIDER
        });
        changed.store(true, std::memory_order_relaxed);
        LogCat::d("tech_provider_entities_rebuilt", "TechProvider entities rebuilt: {} providers",
                  techProviderEntities_.size());
    }
}


glimmer::GameSystemType glimmer::TechProviderSystem::GetGameSystemType() const {
    return GameSystemType::TeachProviderSystem;
}
