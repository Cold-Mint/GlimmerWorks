//
// Created by Cold-Mint on 2025/12/18.
//

#include "ItemContainerComonent.h"

#include "core/world/TileInstancePool.h"
#include "core/world/WorldContext.h"

glimmer::ItemContainerComponent::ItemContainerComponent(size_t capacity)
    : itemContainer_(std::make_unique<ItemContainer>(capacity)) {
}

glimmer::ItemContainerComponent::ItemContainerComponent()
    : ItemContainerComponent(0) {
}

glimmer::ItemContainer *glimmer::ItemContainerComponent::GetItemContainer() const {
    return itemContainer_.get();
}

bool glimmer::ItemContainerComponent::IsSerializable() {
    return true;
}

std::string glimmer::ItemContainerComponent::Serialize() {
    ItemContainerMessage itemContainerMessage;
    itemContainer_->ToMessage(itemContainerMessage);
    return itemContainerMessage.SerializeAsString();
}

void glimmer::ItemContainerComponent::Deserialize(WorldContext *worldContext, const std::string &data) {
    ItemContainerMessage itemContainerMessage;
    if (itemContainerMessage.ParseFromString(data)) {
        itemContainer_->FromMessage(worldContext, itemContainerMessage);
    }
}

GameComponentTypeMessage glimmer::ItemContainerComponent::GetComponentType() {
    return COMPONENT_ITEM_CONTAINER;
}
