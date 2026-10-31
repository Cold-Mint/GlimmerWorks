#pragma once
#include <vector>

#include "RmlUi/Core/EventListener.h"

namespace Rml {
    class Element;
}

namespace glimmer {
    class ItemContainer;

    /**
     * Inventory drag-and-drop listener.
     * 库存拖拽监听器。
     * Supports single-container (backward compatible) and multi-container (e.g. chest)
     * drag-and-drop. Each registered container element is associated with an ItemContainer.
     * 支持单容器（向后兼容）与多容器（如箱子）拖拽。每个注册的容器元素关联一个 ItemContainer。
     */
    class InventoryDragListener : public Rml::EventListener {
        struct Registration {
            ItemContainer *container = nullptr;
            Rml::Element *element = nullptr;
        };

        ItemContainer *defaultContainer_ = nullptr;
        std::vector<Registration> registrations_;

        static Rml::Element *FindSlotElement(Rml::Element *element);

        static int GetSlotIndex(const Rml::Element *slotElement);

        ItemContainer *FindContainer(Rml::Element *slotElement) const;

    public:
        explicit InventoryDragListener(ItemContainer *itemContainer);

        ~InventoryDragListener() override;

        void ProcessEvent(Rml::Event &event) override;

        /**
         * Register a container element using the default container.
         * 使用默认容器注册容器元素。
         */
        void RegisterContainer(Rml::Element *containerElement);

        /**
         * Register a container element with a specific ItemContainer.
         * 使用指定容器注册容器元素。
         */
        void RegisterContainer(Rml::Element *containerElement, ItemContainer *container);

        void UnregisterContainer();
    };
}
