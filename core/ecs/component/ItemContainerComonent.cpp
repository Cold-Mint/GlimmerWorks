//
// Created by Cold-Mint on 2025/12/18.
//

#include "ItemContainerComonent.h"

glimmer::ItemContainer *glimmer::ItemContainerComponent::GetItemContainer() const {
    return itemContainer_.get();
}

u_int32_t glimmer::ItemContainerComponent::GetId() {
    return COMPONENT_ID_ITEM_CONTAINER;
}
