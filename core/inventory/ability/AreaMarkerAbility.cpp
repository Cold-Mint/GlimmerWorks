//
// Created by Cold-Mint on 2026/2/26.
//

#include "AreaMarkerAbility.h"

#include "core/ecs/component/TileLayerComponent.h"
#include "../../world/WorldContext.h"
#include "core/ecs/component/AreaMarkerComponent.h"


glimmer::AreaMarkerAbility::AreaMarkerAbility(const AppContext *appContext,
                                              const AbilityConfig &abilityConfigMessage) : ItemAbility(
    appContext, abilityConfigMessage) {
}

void glimmer::AreaMarkerAbility::OnUse(WorldContext *worldContext, GameEntity::ID user,
                                       const AbilityConfig &abilityConfig,
                                       std::unordered_set<std::string> &popupAbility) {
    auto tileLayerEntityList = worldContext->GetEntityIDWithComponents<
        TileLayerComponent, AreaMarkerComponent>();
    for (const auto &gameEntity: tileLayerEntityList) {
        auto *tileLayerComponent = worldContext->GetComponent<TileLayerComponent>(
            gameEntity);
        auto *areaMarkerComponent = worldContext->GetComponent<AreaMarkerComponent>(gameEntity);
        if (tileLayerComponent == nullptr || areaMarkerComponent == nullptr) {
            continue;
        }
        if (tileLayerComponent->GetTileLayerType() == TileLayerType::Main) {
            areaMarkerComponent->SetPoint(tileLayerComponent->GetFocusPosition());
        }
    }
}

std::string glimmer::AreaMarkerAbility::GetId() const {
    return ABILITY_ID_AREA_MARKER;
}


std::unique_ptr<glimmer::ItemAbility> glimmer::AreaMarkerAbility::Clone() const {
    return std::make_unique<AreaMarkerAbility>(*this);
}
