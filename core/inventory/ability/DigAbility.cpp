//
// Created by coldmint on 2025/12/25.
//

#include "DigAbility.h"
#include "../../world/WorldContext.h"
#include "../../ecs/component/TileLayerComponent.h"
#include "../../ecs/component/Transform2DComponent.h"
#include "../../inventory/TileItem.h"
#include "../../world/ChunkPhysicsHelper.h"

std::string glimmer::DigAbility::GetId() const {
    return ABILITY_ID_DIG;
}

void glimmer::DigAbility::OnUse(AppContext *appContext, WorldContext *worldContext, GameEntity *user) {
    auto tileLayerEntitys = worldContext->GetEntitiesWithComponents<
        TileLayerComponent, Transform2DComponent>();
    for (auto &gameEntity: tileLayerEntitys) {
        auto *tileLayerComponent = worldContext->GetComponent<TileLayerComponent>(
            gameEntity->GetID());
        auto *tileLayerTransform2D = worldContext->GetComponent<Transform2DComponent>(
            gameEntity->GetID());
        if (tileLayerComponent == nullptr || tileLayerTransform2D == nullptr) {
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
            DiggingComponent *diggingComponent = worldContext->GetDiggingComponent();
            const WorldVector2D worldPos = TileLayerComponent::TileToWorld(
                tileLayerTransform2D->GetPosition(), tileVector2D);
            if (diggingComponent->GetPosition() != worldPos) {
                diggingComponent->SetProgress(0.0F);
                diggingComponent->SetPosition(worldPos);
            }
            diggingComponent->SetEfficiency(efficiency_);
            diggingComponent->MarkActive();
        }
    }
}
