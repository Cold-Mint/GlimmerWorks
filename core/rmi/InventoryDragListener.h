#pragma once
#include <cstdint>
#include "RmlUi/Core/EventListener.h"

namespace glimmer {
    class ItemContainer;

    class InventoryDragListener : public Rml::EventListener {
        ItemContainer *itemContainer_ = nullptr;
        Rml::Element *containerElement_ = nullptr;

        static Rml::Element *FindSlotElement(Rml::Element *element);

        static int GetSlotIndex(const Rml::Element *slotElement);

    public:
        explicit InventoryDragListener(ItemContainer *itemContainer);

        ~InventoryDragListener() override = default;

        void ProcessEvent(Rml::Event &event) override;

        void RegisterContainer(Rml::Element *containerElement);

        void UnregisterContainer();
    };
}
