#include "InventoryDragListener.h"

#include "RmlUi/Core/Element.h"
#include "core/inventory/ItemContainer.h"
#include "core/log/LogCat.h"

glimmer::InventoryDragListener::InventoryDragListener(ItemContainer *itemContainer) : itemContainer_(itemContainer) {
}

void glimmer::InventoryDragListener::ProcessEvent(Rml::Event &event) {
    if (event != Rml::EventId::Dragdrop) {
        return;
    }

    if (itemContainer_ == nullptr) {
        return;
    }

    auto *dragElement = static_cast<Rml::Element *>(
        event.GetParameter<void *>("drag_element", nullptr));
    auto *targetElement = event.GetTargetElement();

    if (dragElement == nullptr || targetElement == nullptr) {
        return;
    }

    Rml::Element *sourceSlot = FindSlotElement(dragElement);
    Rml::Element *targetSlot = FindSlotElement(targetElement);

    if (sourceSlot == nullptr || targetSlot == nullptr) {
        return;
    }

    int sourceIndex = GetSlotIndex(sourceSlot);
    int targetIndex = GetSlotIndex(targetSlot);

    if (sourceIndex < 0 || targetIndex < 0 || sourceIndex == targetIndex) {
        return;
    }

    itemContainer_->SwapItem(
        static_cast<uint8_t>(sourceIndex),
        itemContainer_,
        static_cast<uint8_t>(targetIndex));

    LogCat::i("Dragged item from slot ", sourceIndex, " to slot ", targetIndex);
}

void glimmer::InventoryDragListener::RegisterContainer(Rml::Element *containerElement) {
    if (containerElement_ != nullptr) {
        containerElement_->RemoveEventListener(Rml::EventId::Dragdrop, this);
    }
    containerElement_ = containerElement;
    if (containerElement_ != nullptr) {
        containerElement_->AddEventListener(Rml::EventId::Dragdrop, this);
    }
}

void glimmer::InventoryDragListener::UnregisterContainer() {
    if (containerElement_ != nullptr) {
        containerElement_->RemoveEventListener(Rml::EventId::Dragdrop, this);
        containerElement_ = nullptr;
    }
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
