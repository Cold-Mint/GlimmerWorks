//
// Created by Cold-Mint on 2025/12/18.
//

#include "ItemContainerComonent.h"

#include "core/log/LogCat.h"

glimmer::ItemContainer *glimmer::ItemContainerComponent::GetItemContainer() const {
    return itemContainer_.get();
}

bool glimmer::ItemContainerComponent::isSerializable() {
    return true;
}

std::string glimmer::ItemContainerComponent::serialize() {
    ItemContainerMessage itemContainerMessage;
    itemContainer_->ToMessage(itemContainerMessage);
    return itemContainerMessage.SerializeAsString();
}

void glimmer::ItemContainerComponent::
deserialize(AppContext *appContext, WorldContext *worldContext, std::string &data) {
    ItemContainerMessage itemContainerMessage;
    itemContainerMessage.ParseFromString(data);
    itemContainer_->FromMessage(appContext, itemContainerMessage);
}

u_int32_t glimmer::ItemContainerComponent::GetId() {
    return COMPONENT_ID_ITEM_CONTAINER;
}
