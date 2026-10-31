#include "InventoryDragListener.h"

#include "RmlUi/Core/Element.h"
#include "core/inventory/ItemContainer.h"

glimmer::InventoryDragListener::InventoryDragListener(ItemContainer *itemContainer) : defaultContainer_(itemContainer) {
}

glimmer::InventoryDragListener::~InventoryDragListener() {
    UnregisterContainer();
}

void glimmer::InventoryDragListener::ProcessEvent(Rml::Event &event) {
    if (event != Rml::EventId::Dragdrop) {
        return;
    }

    auto *dragElement = static_cast<Rml::Element *>(
        event.GetParameter<void *>("drag_element", nullptr));
    auto *targetElement = event.GetTargetElement();

    if (dragElement == nullptr) {
        return;
    }
    if (targetElement == nullptr) {
        return;
    }

    Rml::Element *sourceSlot = FindSlotElement(dragElement);
    Rml::Element *targetSlot = FindSlotElement(targetElement);

    if (sourceSlot == nullptr) {
        return;
    }
    if (targetSlot == nullptr) {
        return;
    }

    const int sourceIndex = GetSlotIndex(sourceSlot);
    const int targetIndex = GetSlotIndex(targetSlot);

    if (sourceIndex < 0) {
        return;
    }
    if (targetIndex < 0) {
        return;
    }
    if (sourceSlot == targetSlot) {
        return;
    }

    ItemContainer *sourceContainer = FindContainer(sourceSlot);
    ItemContainer *targetContainer = FindContainer(targetSlot);
    if (sourceContainer == nullptr || targetContainer == nullptr) {
        return;
    }
    sourceContainer->SwapItem(
        static_cast<uint8_t>(sourceIndex),
        targetContainer,
        static_cast<uint8_t>(targetIndex));
}

void glimmer::InventoryDragListener::RegisterContainer(Rml::Element *containerElement) {
    RegisterContainer(containerElement, defaultContainer_);
}

void glimmer::InventoryDragListener::RegisterContainer(Rml::Element *containerElement, ItemContainer *container) {
    if (containerElement == nullptr) {
        return;
    }
    for (const auto &registration: registrations_) {
        if (registration.element == containerElement) {
            return;
        }
    }
    containerElement->AddEventListener(Rml::EventId::Dragdrop, this);
    registrations_.push_back({container, containerElement});
}

void glimmer::InventoryDragListener::UnregisterContainer() {
    for (const auto &registration: registrations_) {
        if (registration.element != nullptr) {
            registration.element->RemoveEventListener(Rml::EventId::Dragdrop, this);
        }
    }
    registrations_.clear();
}

glimmer::ItemContainer *glimmer::InventoryDragListener::FindContainer(Rml::Element *slotElement) const {
    for (Rml::Element *current = slotElement; current != nullptr; current = current->GetParentNode()) {
        for (const auto &registration: registrations_) {
            if (registration.element == current) {
                return registration.container;
            }
        }
    }
    return nullptr;
}

Rml::Element *glimmer::InventoryDragListener::FindSlotElement(Rml::Element *element) {
    Rml::Element *current = element;
    while (current != nullptr) {
        if (current->IsClassSet("item_slot")) {
            return current;
        }
        current = current->GetParentNode();
    }
    return nullptr;
}

int glimmer::InventoryDragListener::GetSlotIndex(const Rml::Element *slotElement) {
    if (slotElement == nullptr) {
        return -1;
    }
    auto idxStr = slotElement->GetAttribute<Rml::String>("data-slot-index", "");
    if (idxStr.empty()) {
        return -1;
    }
    return std::atoi(idxStr.c_str());
}
