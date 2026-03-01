//
// Created by Cold-Mint on 2025/12/25.
//

#ifndef GLIMMERWORKS_ITEMSLOTSYSTEM_H
#define GLIMMERWORKS_ITEMSLOTSYSTEM_H

#include "../GameSystem.h"
#include "../component/ItemSlotComponent.h"

namespace glimmer {
    class Item;

    class ItemSlotSystem : public GameSystem {
        void RenderQuantity(SDL_Renderer *renderer, const SDL_FRect &slotDest, int amount) const;

        void RenderTooltip(SDL_Renderer *renderer, const Item *item) const;

        static std::string HumanReadableDisplay(const LangsResources *langsResources,
                                                const VariableDefinition *variableDefinition);

        static bool PositiveAttribute(const VariableDefinition *variableDefinition);

    public:
        explicit ItemSlotSystem(WorldContext *worldContext);

        void Render(SDL_Renderer *renderer) override;

        uint8_t GetRenderOrder() override;

        std::string GetName() override;
    };
}

#endif //GLIMMERWORKS_ITEMSLOTSYSTEM_H
