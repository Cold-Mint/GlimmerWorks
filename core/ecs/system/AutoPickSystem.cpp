//
// Created by coldmint on 2025/12/22.
//

#include "AutoPickSystem.h"

#include "../../world/WorldContext.h"
#include "../component/DroppedItemComponent.h"

void glimmer::AutoPickSystem::Update(float delta) {
    auto entityList = worldContext_->GetEntitiesWithComponents<AutoPickComponent, MagnetComponent,
        ItemContainerComponent>();
    for (auto entity: entityList) {
        auto *magnetComponent = worldContext_->GetComponent<MagnetComponent>(entity->GetID());
        auto *containerComponent = worldContext_->GetComponent<ItemContainerComponent>(
            entity->GetID());
        for (unsigned int entity1: magnetComponent->GetEntities()) {
            DroppedItemComponent *droppedItemComponent = worldContext_->GetComponent<DroppedItemComponent>(entity1);
            if (droppedItemComponent == nullptr) {
                continue;
            }
            auto item = containerComponent->GetItemContainer()->AddItem(droppedItemComponent->ExtractItem());
            if (item == nullptr) {
                worldContext_->RemoveEntity(entity1);
            } else {
                droppedItemComponent->SetItem(std::move(item));
            }
        }
    }
}

std::string glimmer::AutoPickSystem::GetName() {
    return "AutoPickSystem";
}
