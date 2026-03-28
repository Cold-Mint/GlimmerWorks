//
// Created by Cold-Mint on 2025/12/22.
//

#include "AutoPickSystem.h"

#include <strstream>

#include "../../world/WorldContext.h"
#include "../component/DroppedItemComponent.h"
#include "core/ecs/FlowingTextCreator.h"
#include "core/ecs/component/AutoPickComponent.h"
#include "core/ecs/component/MagnetComponent.h"


glimmer::AutoPickSystem::AutoPickSystem(WorldContext *worldContext) : GameSystem(worldContext) {
    RequireComponent<AutoPickComponent>();
    RequireComponent<MagnetComponent>();
    RequireComponent<ItemContainerComponent>();
    ResourceRef ref{};
    ref.SetSelfPackageId(RESOURCE_REF_CORE);
    ref.SetResourceType(RESOURCE_TYPE_AUDIO);
    ref.SetResourceKey("sfx/pick_item");
    AppContext *appContext = worldContext->GetAppContext();
    if (appContext != nullptr) {
        pickItemSFX_ = appContext->GetResourceLocator()->FindAudio(ref);
    }
}

void glimmer::AutoPickSystem::Update(float delta) {
    if (remainingTime_ <= 0) {
        if (!frameItemCounts_.empty()) {
            std::stringstream stringStream{};
            for (auto &pair: frameItemCounts_) {
                stringStream << pair.first;
                if (pair.second > 1) {
                    stringStream << " * " << pair.second << "\n";
                }
            }

            FlowingTextCreator flowingTextCreator(worldContext_, stringStream.str(), lastPosition);
            flowingTextCreator.LoadTemplateComponents(worldContext_->CreateEntity());
            frameItemCounts_.clear();
        }
        remainingTime_ = MERGE_DURATION;
    }
    remainingTime_ -= delta;

    auto entityList = worldContext_->GetEntityIDWithComponents<AutoPickComponent, MagnetComponent,
        ItemContainerComponent>();
    for (auto entity: entityList) {
        auto *magnetComponent = worldContext_->GetComponent<MagnetComponent>(entity);
        auto *containerComponent = worldContext_->GetComponent<ItemContainerComponent>(
            entity);
        auto &entities = magnetComponent->GetEntities();
        for (GameEntity::ID entityId: entities) {
            auto *transform2DComponent = worldContext_->GetComponent<Transform2DComponent>(entityId);
            if (transform2DComponent == nullptr) {
                continue;
            }
            lastPosition = transform2DComponent->GetPosition();

            auto *droppedItemComponent = worldContext_->GetComponent<DroppedItemComponent>(entityId);
            if (droppedItemComponent == nullptr) {
                continue;
            }

            auto extractItem = droppedItemComponent->ExtractItem();
            if (extractItem == nullptr) {
                continue;
            }

            const std::string &itemName = extractItem->GetName();
            frameItemCounts_[itemName] += extractItem->GetAmount();

            auto item = containerComponent->GetItemContainer()->AddItem(std::move(extractItem));
            if (item == nullptr) {
                worldContext_->GetAppContext()->GetAudioManager()->TryPlayFree(AMBIENT, pickItemSFX_.get(), 0);
            }
            worldContext_->RemoveEntity(entityId);
        }
    }
}


std::string glimmer::AutoPickSystem::GetName() {
    return "glimmer.AutoPickSystem";
}
