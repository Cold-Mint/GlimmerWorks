//
// Created by coldmint on 2025/12/22.
//

#include "AutoPickSystem.h"

#include "../../world/WorldContext.h"
#include "../component/DroppedItemComponent.h"
#include "core/ecs/component/AutoPickComponent.h"
#include "core/ecs/component/MagnetComponent.h"

glimmer::AutoPickSystem::AutoPickSystem(AppContext *appContext, WorldContext *worldContext)
    : GameSystem(appContext, worldContext) {
    RequireComponent<AutoPickComponent>();
    RequireComponent<MagnetComponent>();
    RequireComponent<ItemContainerComponent>();
}

void glimmer::AutoPickSystem::Update(float delta) {
    auto entityList = worldContext_->GetEntityIDWithComponents<AutoPickComponent, MagnetComponent,
        ItemContainerComponent>();
    for (auto entity: entityList) {
        auto *magnetComponent = worldContext_->GetComponent<MagnetComponent>(entity);
        auto *containerComponent = worldContext_->GetComponent<ItemContainerComponent>(
            entity);
        for (GameEntity::ID entityId: magnetComponent->GetEntities()) {
            auto *droppedItemComponent = worldContext_->GetComponent<DroppedItemComponent>(entityId);
            if (droppedItemComponent == nullptr) {
                continue;
            }
            auto item = containerComponent->GetItemContainer()->AddItem(droppedItemComponent->ExtractItem());
            if (item == nullptr) {
                worldContext_->RemoveEntity(entityId);
            } else {
                droppedItemComponent->SetItem(std::move(item));
            }
        }
    }
}

std::string glimmer::AutoPickSystem::GetName() {
    return "glimmer::AutoPickSystem";
}
