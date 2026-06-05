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
#include "DigAbility.h"

#include "MiningRangeData.h"
#include "../../world/WorldContext.h"
#include "../../ecs/component/TileLayerComponent.h"
#include "../../ecs/component/Transform2DComponent.h"
#include "../../inventory/TileItem.h"


glimmer::DigAbility::DigAbility(const AbilityConfig& abilityConfig) : ItemAbility(
    abilityConfig)
{
}

std::string glimmer::DigAbility::GetId() const
{
    return ABILITY_ID_DIG;
}

void glimmer::DigAbility::OnUse(WorldContext* worldContext, uint32_t user, const AbilityConfig* abilityConfig,
                                std::unordered_set<std::string>& popupAbility)
{
    popupAbility.emplace(GetId());
    if (abilityConfig == nullptr)
    {
        return;
    }
    if (abilityConfig->mineAbleLayer == 0)
    {
        return;
    }
    EntityManager* entityManager = worldContext->GetEntityManager();
    if (entityManager == nullptr)
    {
        return;
    }
    auto playerEntity = worldContext->GetEntityShortCut()->GetPlayer();
    if (WorldContext::IsEmptyEntityId(playerEntity))
    {
        return;
    }
    auto playerTransform = entityManager->GetComponent<Transform2DComponent>(playerEntity);
    if (playerTransform == nullptr)
    {
        return;
    }
    EntityShortCut* entityShortCut = worldContext->GetEntityShortCut();
    if (entityShortCut == nullptr)
    {
        return;
    }
    const WorldVector2D playerWorldPos = playerTransform->GetPosition();
    DiggingComponent* diggingComponent = entityShortCut->GetDiggingComponent();
    if (diggingComponent == nullptr)
    {
        return;
    }
    auto tileLayerEntities = entityManager->GetEntityIDWithComponents({COMPONENT_TILE_LAYER});
    std::sort(tileLayerEntities.begin(), tileLayerEntities.end());
    for (const auto& gameEntity : tileLayerEntities)
    {
        auto* tileLayerComponent = entityManager->GetComponent<TileLayerComponent>(
            gameEntity);
        if (tileLayerComponent == nullptr)
        {
            continue;
        }
        const TileLayerType layerType = tileLayerComponent->GetTileLayerType();
        if (abilityConfig->mineAbleLayer & layerType)
        {
            const TileVector2D& tileVector2D = tileLayerComponent->GetFocusPosition();
            if ((TileLayerComponent::TileToWorld(tileVector2D) + WorldVector2D{HALF_TILE_SIZE, HALF_TILE_SIZE}).
                Distance(playerWorldPos) / TILE_SIZE > abilityConfig
                ->miningRange)
            {
                continue;
            }
            const Tile* tile = tileLayerComponent->GetSelfLayerTile(
                tileVector2D);
            if (tile == nullptr)
            {
                continue;
            }
            if (!tile->IsBreakable())
            {
                continue;
            }
            if (diggingComponent->GetStartPosition() != tileVector2D)
            {
                //Change the starting point of the excavation and recalculate the progress.
                //挖掘起点改变，重新计算进度。
                miningRangeData_.Reset();
                diggingComponent->SetChainMiningRadius(abilityConfig->chainMiningRadius);
                miningRangeData_.
                    CalculateChainMining(tileLayerComponent, tileVector2D, abilityConfig->chainMiningRadius);
                diggingComponent->SetPrecisionMining(abilityConfig->enablePrecisionMining);
                size_t pointCount = miningRangeData_.GetPointsCount();
                if (pointCount == 0)
                {
                    //If no exploitable tiles are found, then calculate the default excavation range.
                    //如果没有发现可挖掘的瓦片，那么计算默认的挖掘范围。
                    miningRangeData_.CalculateMining(tileLayerComponent, tileVector2D);
                }
                diggingComponent->SetEfficiency(abilityConfig->miningEfficiency);
                diggingComponent->SetMiningRangeData(&miningRangeData_);
                diggingComponent->SetProgress(0.0F);
                diggingComponent->SetStartPosition(tileVector2D);
            }
            //efficiency
            //工具效率
            diggingComponent->SetLayerType(layerType);
            if (miningRangeData_.GetPointsCount() > 0)
            {
                //If there are any exploitable tiles, then activate the mining module.
                //如果有可挖掘的瓦片，那么激活挖掘组建。
                diggingComponent->MarkActive();
            }
            // Must break the loop: tool can destroy multiple layers (e.g. mineAbleLayer = 3: ground + background).
            // If current layerType = 1 (ground) is destroyed, stop checking next layers.
            // Do NOT destroy ground and background simultaneously.
            // 必须终止循环：该工具能够破坏多层（例如，mineAbleLayer = 3：地面 + 背景）。
            // 如果当前的 layerType（即地面层，值为 1）被破坏，则停止检查后续的层。
            // 不要同时破坏地面和背景层。
            break;
        }
    }
}


std::unique_ptr<glimmer::ItemAbility> glimmer::DigAbility::Clone() const
{
    return std::make_unique<DigAbility>(*this);
}
