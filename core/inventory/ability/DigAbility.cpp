//
// Created by coldmint on 2025/12/25.
//

#include "DigAbility.h"
#include "../../world/WorldContext.h"
#include "../../ecs/component/TileLayerComponent.h"
#include "../../ecs/component/Transform2DComponent.h"
#include "../../inventory/TileItem.h"

glimmer::DigAbility::DigAbility(const VariableConfig &abilityData) : ItemAbility(
    abilityData) {
    auto efficiencyVariable = abilityData.FindVariable("efficiency");
    if (efficiencyVariable != nullptr) {
        efficiency_ = efficiencyVariable->AsFloat();
    }

    auto digRange = abilityData.FindVariable("digRange");
    if (digRange != nullptr) {
        digRange_ = digRange->AsInt();
    }
}

std::string glimmer::DigAbility::GetId() const {
    return ABILITY_ID_DIG;
}

void glimmer::DigAbility::OnUse(WorldContext *worldContext, GameEntity::ID user) {
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
            //Distance (unit: number of tile grids)
            //距离（单位：瓦片格子数）
            if (TileLayerComponent::TileToWorldCenter(tileVector2D).Distance(playerWorldPos) / TILE_SIZE > static_cast
                <float>(digRange_)) {
                continue;
            }
            DiggingComponent *diggingComponent = worldContext->GetDiggingComponent();
            const WorldVector2D tileWorldPos = TileLayerComponent::TileToWorld(tileVector2D);
            if (diggingComponent->GetPosition() != tileWorldPos) {
                diggingComponent->SetProgress(0.0F);
                diggingComponent->SetPosition(tileWorldPos);
            }
            diggingComponent->SetEfficiency(efficiency_);
            diggingComponent->MarkActive();
        }
    }
}

std::unique_ptr<glimmer::ItemAbility> glimmer::DigAbility::Clone() const {
    return std::make_unique<DigAbility>(*this);
}
