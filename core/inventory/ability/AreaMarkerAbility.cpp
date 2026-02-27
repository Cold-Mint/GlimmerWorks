//
// Created by coldmint on 2026/2/26.
//

#include "AreaMarkerAbility.h"

#include "core/ecs/component/TileLayerComponent.h"
#include "../../world/WorldContext.h"
#include "core/ecs/component/AreaMarkerComponent.h"


glimmer::AreaMarkerAbility::AreaMarkerAbility(const AppContext *appContext,
                                              const VariableConfig &abilityData) : ItemAbility(
    appContext, abilityData) {
    auto areaMarkerBorderVariable = abilityData.FindVariable("areaMarkerBorder");
    if (areaMarkerBorderVariable != nullptr) {
        ResourceRef resourceRef;
        areaMarkerBorderVariable->AsResourceRef(resourceRef);
        areaMarkerBorderColor_ = appContext->GetResourceLocator()->FindColorResource(resourceRef)->ToSDLColor();
    }

    auto areaMarkerFullVariable = abilityData.FindVariable("areaMarkerFull");
    if (areaMarkerFullVariable != nullptr) {
        ResourceRef resourceRef;
        areaMarkerFullVariable->AsResourceRef(resourceRef);
        areaMarkerFullColor_ = appContext->GetResourceLocator()->FindColorResource(resourceRef)->ToSDLColor();
    }
}

std::string glimmer::AreaMarkerAbility::GetId() const {
    return ABILITY_ID_AREA_MARKER;
}

void glimmer::AreaMarkerAbility::OnUse(WorldContext *worldContext, GameEntity::ID user) {
    auto tileLayerEntityList = worldContext->GetEntityIDWithComponents<
        TileLayerComponent, AreaMarkerComponent>();
    for (const auto &gameEntity: tileLayerEntityList) {
        auto *tileLayerComponent = worldContext->GetComponent<TileLayerComponent>(
            gameEntity);
        auto *areaMarkerComponent = worldContext->GetComponent<AreaMarkerComponent>(gameEntity);
        if (tileLayerComponent == nullptr || areaMarkerComponent == nullptr) {
            continue;
        }
        areaMarkerComponent->SetAreaMarkerBorderColor(areaMarkerBorderColor_);
        areaMarkerComponent->SetAreaMarkerFullColor(areaMarkerFullColor_);
        if (tileLayerComponent->GetTileLayerType() == TileLayerType::Main) {
            areaMarkerComponent->SetPoint(tileLayerComponent->GetFocusPosition());
        }
    }
}

std::unique_ptr<glimmer::ItemAbility> glimmer::AreaMarkerAbility::Clone() const {
    return std::make_unique<AreaMarkerAbility>(*this);
}
