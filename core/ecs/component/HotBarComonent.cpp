//
// Created by Cold-Mint on 2025/12/18.
//

#include "HotBarComonent.h"

#include "core/Constants.h"


int glimmer::HotBarComponent::GetSelectedSlot() const {
    return selectedSlot_;
}

void glimmer::HotBarComponent::SetSelectedSlot(int selectedSlot) {
    if (selectedSlot < 0) {
        selectedSlot = maxSlot_ - 1;
    } else if (selectedSlot >= maxSlot_) {
        selectedSlot = 0;
    }
    selectedSlot_ = selectedSlot;
}

int glimmer::HotBarComponent::GetMaxSlot() const {
    return maxSlot_;
}

u_int32_t glimmer::HotBarComponent::GetId() {
    return COMPONENT_ID_HOTBAR;
}
