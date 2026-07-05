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

#include "core/ecs/component/PlayerComponent.h"
#include "core/ecs/component/TechProviderComponent.h"
#include "core/world/WorldContext.h"

glimmer::TechProviderSystem::TechProviderSystem(WorldContext* worldContext)
    : GameSystem(worldContext)
{
    WatchComponent(COMPONENT_TRANSFORM_2D);
    WatchComponent(COMPONENT_TECH_PROVIDER);
    WatchComponent(COMPONENT_PLAYER);
    Init();
}

void glimmer::TechProviderSystem::OnActivationChanged(bool activeStatus)
{
    EntityManager* entityManager = GetEntityManager();
    if (!activeStatus)
    {
        if (WorldContext::IsEmptyEntityId(player_))
        {
            return;
        }
        auto playerComponent = entityManager->GetComponent<PlayerComponent>(player_);
        if (playerComponent == nullptr)
        {
            return;
        }
        playerComponent->ResetTechnologyMap();
    }
}

void glimmer::TechProviderSystem::OnFrameStart()
{
    EntityManager* entityManager = GetEntityManager();
    if (!changed)
    {
        return;
    }
    if (WorldContext::IsEmptyEntityId(player_))
    {
        return;
    }
    auto playerTransform2DComponent = entityManager->GetComponent<Transform2DComponent>(player_);
    if (playerTransform2DComponent == nullptr)
    {
        return;
    }
    auto playerComponent = entityManager->GetComponent<PlayerComponent>(player_);
    if (playerComponent == nullptr)
    {
        return;
    }
    playerComponent->ResetTechnologyMap();
    for (GameEntityID techProviderEntity : techProviderEntities_)
    {
        auto providerTransform2DComponent = entityManager->GetComponent<Transform2DComponent>(
            techProviderEntity);
        if (providerTransform2DComponent == nullptr)
        {
            continue;
        }
        if (playerTransform2DComponent->GetPosition().Distance(
            providerTransform2DComponent->GetPosition()) / TILE_SIZE > WORK_STATION_RANGE)
        {
            continue;
        }
        auto techProviderComponent = entityManager->GetComponent<TechProviderComponent>(
            techProviderEntity);
        if (techProviderComponent == nullptr)
        {
            continue;
        }
        playerComponent->SetTechnology(techProviderComponent->GetRecipeGroup(),
                                       techProviderComponent->GetTechnologyLevel());
    }
    changed = false;
}

void glimmer::TechProviderSystem::OnWatchedComponentChanged(GameComponentTypeMessage gameComponentType, uint32_t count)
{
    EntityShortCut* entityShortCut = GetEntityShortCut();
    EntityManager* entityManager = GetEntityManager();
    if (gameComponentType == COMPONENT_TRANSFORM_2D)
    {
        transform2DCount_ = count;
    }
    if (gameComponentType == COMPONENT_PLAYER)
    {
        player_ = entityShortCut->GetPlayer();
    }
    if (gameComponentType == COMPONENT_TECH_PROVIDER)
    {
        techProviderCount_ = count;
    }
    if (transform2DCount_ > 0 && techProviderCount_ > 0)
    {
        techProviderEntities_.clear();
        techProviderEntities_ = entityManager->GetEntityIDWithComponents({
            COMPONENT_TRANSFORM_2D, COMPONENT_TECH_PROVIDER
        });
        changed = true;
    }
}


glimmer::GameSystemType glimmer::TechProviderSystem::GetGameSystemType() const
{
    return GameSystemType::TeachProviderSystem;
}
