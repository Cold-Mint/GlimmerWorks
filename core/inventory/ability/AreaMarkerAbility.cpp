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
#include "AreaMarkerAbility.h"

#include "core/ecs/component/TileLayerComponent.h"
#include "core/world/WorldContext.h"
#include "core/ecs/component/AreaMarkerComponent.h"


glimmer::AreaMarkerAbility::AreaMarkerAbility(
    const AbilityConfig& abilityConfigMessage) : ItemAbility(
    abilityConfigMessage)
{
}

void glimmer::AreaMarkerAbility::OnUse(WorldContext* worldContext, uint32_t user,
                                       const AbilityConfig* abilityConfig,
                                       std::unordered_set<std::string>& popupAbility)
{
    auto entityManager = worldContext->GetEntityManager();
    auto tileLayerEntityList = entityManager->GetEntityIDWithComponents({COMPONENT_TILE_LAYER, COMPONENT_AREA_MARKER});
    if (tileLayerEntityList.empty())
    {
        return;
    }
    const uint32_t gameEntity = tileLayerEntityList[0];
    const auto tileLayerComponent = entityManager->GetComponent<TileLayerComponent>(
        gameEntity);
    auto areaMarkerComponent = entityManager->GetComponent<AreaMarkerComponent>(gameEntity);
    if (tileLayerComponent == nullptr || areaMarkerComponent == nullptr)
    {
        return;
    }
    areaMarkerComponent->SetPoint(tileLayerComponent->GetFocusPosition());
}

const std::string& glimmer::AreaMarkerAbility::GetId() const
{
    return ABILITY_ID_AREA_MARKER;
}


std::unique_ptr<glimmer::ItemAbility> glimmer::AreaMarkerAbility::Clone() const
{
    return std::make_unique<AreaMarkerAbility>(*this);
}
