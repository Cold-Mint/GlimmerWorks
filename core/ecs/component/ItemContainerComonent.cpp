//
// Created by coldmint on 2025/12/18.
//

#include "ItemContainerComonent.h"

glimmer::ItemContainer *glimmer::ItemContainerComponent::GetItemContainer() const {
    return itemContainer_.get();
}
