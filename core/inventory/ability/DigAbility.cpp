//
// Created by Cold-Mint on 2025/12/25.
//

#include "DigAbility.h"

#include "MiningRangeData.h"
#include "../../world/WorldContext.h"
#include "../../ecs/component/TileLayerComponent.h"
#include "../../ecs/component/Transform2DComponent.h"
#include "../../inventory/TileItem.h"


glimmer::DigAbility::DigAbility(const AppContext *appContext, const AbilityConfig &abilityConfig) : ItemAbility(
    appContext, abilityConfig) {
}

std::string glimmer::DigAbility::GetId() const {
    return ABILITY_ID_DIG;
}

void glimmer::DigAbility::OnUse(WorldContext *worldContext, GameEntity::ID user, const AbilityConfig &abilityConfig,
                                std::unordered_set<std::string> &popupAbility) {
    popupAbility.emplace(GetId());
    if (abilityConfig.mineAbleLayer == 0) {
        return;
    }
    auto tileLayerEntityList = worldContext->GetEntityIDWithComponents<
        TileLayerComponent>();
    auto playerEntity = worldContext->GetPlayerEntity();
    if (WorldContext::IsEmptyEntityId(playerEntity)) {
        return;
    }
    auto playerTransform = worldContext->GetComponent<Transform2DComponent>(playerEntity);
    if (playerTransform == nullptr) {
        return;
    }
    const WorldVector2D playerWorldPos = playerTransform->GetPosition();
    DiggingComponent *diggingComponent = worldContext->GetDiggingComponent();
    if (diggingComponent == nullptr) {
        return;
    }
    for (const auto &gameEntity: tileLayerEntityList) {
        auto *tileLayerComponent = worldContext->GetComponent<TileLayerComponent>(
            gameEntity);
        if (tileLayerComponent == nullptr) {
            continue;
        }
        TileLayerType layerType = tileLayerComponent->GetTileLayerType();
        if (abilityConfig.mineAbleLayer & layerType) {
            TileVector2D tileVector2D = tileLayerComponent->GetFocusPosition();
            const Tile *tile = tileLayerComponent->GetSelfLayerTile(
                tileVector2D);
            if (tile == nullptr) {
                continue;
            }
            if (!tile->IsBreakable()) {
                continue;
            }
            if (TileLayerComponent::TileToWorldCenter(tileVector2D).Distance(playerWorldPos) / TILE_SIZE > abilityConfig
                .miningRange) {
                continue;
            }
            const WorldVector2D tileWorldPos = TileLayerComponent::TileToWorld(tileVector2D);
            if (diggingComponent->GetStartPosition() != tileWorldPos) {
                //Change the starting point of the excavation and recalculate the progress.
                //挖掘起点改变，重新计算进度。
                miningRangeData_.Reset();
                diggingComponent->SetChainMiningRadius(abilityConfig.chainMiningRadius);
                miningRangeData_.
                        CalculateChainMining(tileWorldPos, tileLayerComponent, abilityConfig.chainMiningRadius);
                diggingComponent->SetPrecisionMining(abilityConfig.enablePrecisionMining);
                if (miningRangeData_.GetPoints().empty()) {
                    //If no exploitable tiles are found, then calculate the default excavation range.
                    //如果没有发现可挖掘的瓦片，那么计算默认的挖掘范围。
                    miningRangeData_.CalculateMining(tileWorldPos, tileLayerComponent);
                }
                diggingComponent->SetEfficiency(abilityConfig.miningEfficiency);
                diggingComponent->SetMiningRangeData(&miningRangeData_);
                diggingComponent->SetProgress(0.0F);
                diggingComponent->SetStartPosition(tileWorldPos);
            }
            //efficiency
            //工具效率
            diggingComponent->SetLayerType(layerType);
            if (!miningRangeData_.GetPoints().empty()) {
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


std::unique_ptr<glimmer::ItemAbility> glimmer::DigAbility::Clone() const {
    return std::make_unique<DigAbility>(*this);
}
