//
// Created by coldmint on 2025/12/25.
//

#include "DigAbility.h"

#include "MiningRangeData.h"
#include "../../world/WorldContext.h"
#include "../../ecs/component/TileLayerComponent.h"
#include "../../ecs/component/Transform2DComponent.h"
#include "../../inventory/TileItem.h"

glimmer::DigAbility::DigAbility(const AppContext *appContext, const VariableConfig &abilityData) : ItemAbility(
    appContext, abilityData) {
}

std::string glimmer::DigAbility::GetId() const {
    return ABILITY_ID_DIG;
}

void glimmer::DigAbility::OnUse(WorldContext *worldContext, GameEntity::ID user, const VariableConfig &abilityData,
                                std::unordered_set<std::string> &popupAbility) {
    popupAbility.emplace(GetId());
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
    for (const auto &gameEntity: tileLayerEntityList) {
        auto *tileLayerComponent = worldContext->GetComponent<TileLayerComponent>(
            gameEntity);
        if (tileLayerComponent == nullptr) {
            continue;
        }
        if (tileLayerComponent->GetTileLayerType() == TileLayerType::Main) {
            TileVector2D tileVector2D = tileLayerComponent->GetFocusPosition();
            const Tile *tile = tileLayerComponent->GetTile(
                tileVector2D);
            if (tile == nullptr) {
                continue;
            }
            if (!tile->breakable) {
                continue;
            }
            //dig Range(Unit: Number of Tile Squares)
            //挖掘距离（单位：瓦片格数）
            auto digRangeVariable = abilityData.FindVariable("digRange");
            int digRange = 5;
            if (digRangeVariable != nullptr) {
                digRange = digRangeVariable->AsInt();
                if (digRange < 0) {
                    digRange = 0;
                }
            }
            if (TileLayerComponent::TileToWorldCenter(tileVector2D).Distance(playerWorldPos) / TILE_SIZE > static_cast
                <float>(digRange)) {
                continue;
            }
            DiggingComponent *diggingComponent = worldContext->GetDiggingComponent();
            const WorldVector2D tileWorldPos = TileLayerComponent::TileToWorld(tileVector2D);
            if (diggingComponent->GetStartPosition() != tileWorldPos) {
                //Change the starting point of the excavation and recalculate the progress.
                //挖掘起点改变，重新计算进度。
                miningRangeData_.Reset();
                auto chainMiningRadiusVariable = abilityData.FindVariable("chainMiningRadius");
                int chainMiningRadius = 0;
                if (chainMiningRadiusVariable != nullptr) {
                    chainMiningRadius = chainMiningRadiusVariable->AsInt();
                }
                diggingComponent->SetChainMiningRadius(chainMiningRadius);
                miningRangeData_.CalculateChainMining(tileWorldPos, tileLayerComponent, chainMiningRadius);
                auto precisionMiningVariable = abilityData.FindVariable("precisionMining");
                bool precisionMining = false;
                if (precisionMiningVariable != nullptr) {
                    precisionMining = precisionMiningVariable->AsBool();
                }
                diggingComponent->SetPrecisionMining(precisionMining);
                if (miningRangeData_.GetPoints().empty()) {
                    //If no exploitable tiles are found, then calculate the default excavation range.
                    //如果没有发现可挖掘的瓦片，那么计算默认的挖掘范围。
                    miningRangeData_.CalculateMining(tileWorldPos, tileLayerComponent);
                }
                diggingComponent->SetMiningRangeData(&miningRangeData_);
                diggingComponent->SetProgress(0.0F);
                diggingComponent->SetStartPosition(tileWorldPos);
            }
            //efficiency
            //工具效率
            auto efficiencyVariable = abilityData.FindVariable("efficiency");
            float efficiency = 1.0F;
            if (efficiencyVariable != nullptr) {
                efficiency = efficiencyVariable->AsFloat();
                if (efficiency < 0.0F) {
                    efficiency = 0.0F;
                }
            }
            diggingComponent->SetEfficiency(efficiency);
            if (!miningRangeData_.GetPoints().empty()) {
                //If there are any exploitable tiles, then activate the mining module.
                //如果有可挖掘的瓦片，那么激活挖掘组建。
                diggingComponent->MarkActive();
            }
        }
    }
}

std::unique_ptr<glimmer::ItemAbility> glimmer::DigAbility::Clone() const {
    return std::make_unique<DigAbility>(*this);
}
