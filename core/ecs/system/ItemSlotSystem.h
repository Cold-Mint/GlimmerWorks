//
// Created by Cold-Mint on 2025/12/25.
//

#pragma once
#include "core/ecs/GameSystem.h"
#include "core/ecs/component/ItemSlotComponent.h"

namespace glimmer {
    class Item;

    class ItemSlotSystem : public GameSystem {
        void RenderQuantity(SDL_Renderer *renderer, const SDL_FRect &slotDest, int amount) const;

        void RenderTooltip(SDL_Renderer *renderer, const Item *item) const;

    public:
        explicit ItemSlotSystem(WorldContext *worldContext);

        void Render(SDL_Renderer *renderer) override;

        bool HandleEvent(const SDL_Event &event) override;

        uint8_t GetRenderOrder() override;

        std::string GetName() override;
    };
}
