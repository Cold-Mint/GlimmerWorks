//
// Created by Cold-Mint on 2025/12/18.
//

#include "HotBarSystem.h"
#include "../component/HotBarComonent.h"
#include "../../inventory/Item.h"
#include "../../world/WorldContext.h"


void glimmer::HotBarSystem::Render(SDL_Renderer *renderer) {
    const auto entities = worldContext_->GetEntitiesWithComponents<PlayerControlComponent, HotBarComponent,
        ItemContainerComponent>();
    SDL_Color oldColor;
    SDL_GetRenderDrawColor(renderer, &oldColor.r, &oldColor.g, &oldColor.b, &oldColor.a);

    for (auto &entity: entities) {
        const auto *hotBar = worldContext_->GetHotBarComponent();
        const auto *itemContainerComp = worldContext_->GetComponent<ItemContainerComponent>(entity->GetID());
        if (hotBar == nullptr || itemContainerComp == nullptr) {
            continue;
        }

        Vector2D pos = hotBar->GetPosition();
        const float slotSize = 40.0F * appContext_->GetConfig()->window.uiScale;
        float padding = 8.0F * appContext_->GetConfig()->window.uiScale;

        float startX = pos.x;
        float startY = pos.y;

        for (int i = 0; i < hotBar->GetMaxSlot(); ++i) {
            SDL_FRect rect = {startX + i * (slotSize + padding), startY, slotSize, slotSize};

            // Draw Slot Frame
            SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
            SDL_RenderRect(renderer, &rect);

            // Draw Item
            if (auto *container = itemContainerComp->GetItemContainer()) {
                Item *item = container->GetItem(i);
                if (item) {
                    auto texture = item->GetIcon();
                    if (texture) {
                        SDL_RenderTexture(renderer, texture.get(), nullptr, &rect);
                    }
                }
            }

            // Draw Selection Highlight
            if (i == hotBar->GetSelectedSlot()) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Yellow
                SDL_FRect selRect = {rect.x - 2, rect.y - 2, rect.w + 4, rect.h + 4};
                SDL_RenderRect(renderer, &selRect);
            }
        }
    }
    SDL_SetRenderDrawColor(renderer, oldColor.r, oldColor.g, oldColor.b, oldColor.a);
}

uint8_t glimmer::HotBarSystem::GetRenderOrder() {
    return RENDER_ORDER_DEBUG_HOTBAR;
}

std::string glimmer::HotBarSystem::GetName() {
    return "glimmer.HotBarSystem";
}
