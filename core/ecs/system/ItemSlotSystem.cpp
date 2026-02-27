//
// Created by Cold-Mint on 2025/12/25.
//

#include "ItemSlotSystem.h"
#include "../../inventory/Item.h"
#include "../../world/WorldContext.h"
#include "../../inventory/DragAndDrop.h"
#include "SDL3_ttf/SDL_ttf.h"
#include <SDL3/SDL.h>
#include <string>
#include <algorithm>

#include "core/ecs/component/GuiTransform2DComponent.h"

void glimmer::ItemSlotSystem::Render(SDL_Renderer *renderer) {
    AppContext *appContext = worldContext_->GetAppContext();
    const auto entities = worldContext_->GetEntityIDWithComponents<ItemSlotComponent, GuiTransform2DComponent>();
    float mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    const Item *hoveredItem = nullptr;
    DragAndDrop *dragAndDrop = appContext->GetDragAndDrop();
    for (auto &entity: entities) {
        const auto slotComp = worldContext_->GetComponent<ItemSlotComponent>(entity);
        const auto transform = worldContext_->GetComponent<GuiTransform2DComponent>(entity);

        const Vector2D pos = transform->GetPosition();
        const Vector2D size = transform->GetSize();
        const auto itemContainer = slotComp->GetItemContainer();
        if (itemContainer == nullptr) {
            continue;
        }

        int slotIndex = slotComp->GetSlotIndex();
        Item *item = itemContainer->GetItem(slotIndex);
        const SDL_FRect rect = {pos.x, pos.y, size.x, size.y};
        bool isHovered = mouseX >= rect.x && mouseX <= rect.x + rect.w &&
                         mouseY >= rect.y && mouseY <= rect.y + rect.h;
        slotComp->SetHovered(isHovered);
        if (isHovered && item != nullptr) {
            hoveredItem = item;
        }

        dragAndDrop->DrawSlot(appContext, renderer, pos, size, item, slotComp->IsSelected(),
                              [&](const DragState &state) {
                                  if (state.sourceType != DragSourceType::INVENTORY) {
                                      return;
                                  }
                                  if (state.sourceContainer != nullptr) {
                                      itemContainer->SwapItem(slotIndex, state.sourceContainer, state.sourceIndex);
                                  }
                              },
                              [&] {
                                  dragAndDrop->BeginDrag(DragSourceType::INVENTORY,
                                                         itemContainer, slotIndex,
                                                         item);
                              },
                              nullptr
        );
    }

    if (dragAndDrop->IsDragging()) {
        dragAndDrop->RenderCombined(renderer, mouseX, mouseY, appContext->GetConfig()->window.uiScale);
    }

    if (hoveredItem && !dragAndDrop->IsDragging()) {
        RenderTooltip(renderer, hoveredItem);
    }
    AppContext::RestoreColorRenderer(renderer);
}

void glimmer::ItemSlotSystem::RenderQuantity(SDL_Renderer *renderer, const SDL_FRect &slotDest, int amount) const {
    const std::string text = std::to_string(amount);
    AppContext *appContext = worldContext_->GetAppContext();
    SDL_Surface *surface = TTF_RenderText_Blended(appContext->GetFont(), text.c_str(), text.length(),
                                                  appContext->GetPreloadColors()->textColor);
    if (surface) {
        if (SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface)) {
            auto textW = static_cast<float>(surface->w);
            auto textH = static_cast<float>(surface->h);
            SDL_FRect dst = {
                slotDest.x + slotDest.w - textW - 2,
                slotDest.y + slotDest.h - textH - 2,
                textW,
                textH
            };
            SDL_RenderTexture(renderer, texture, nullptr, &dst);
            SDL_DestroyTexture(texture);
        }
        SDL_DestroySurface(surface);
    }
}

void glimmer::ItemSlotSystem::RenderTooltip(SDL_Renderer *renderer, const Item *item) const {
    if (!item) return;

    float mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    const std::string nameInfo = item->GetName();
    const std::string descInfo = item->GetDescription();

    AppContext *appContext = worldContext_->GetAppContext();
    SDL_Surface *sName = TTF_RenderText_Blended(appContext->GetFont(), nameInfo.c_str(), nameInfo.length(),
                                                appContext->GetPreloadColors()->textColor);
    SDL_Surface *sDesc = TTF_RenderText_Blended(appContext->GetFont(), descInfo.c_str(), descInfo.length(),
                                                appContext->GetPreloadColors()->textColor);

    float maxWidth = 0;
    float totalHeight = 0;
    const float padding = 5.0f;
    const float lineSpacing = 2.0f;

    if (sName) {
        maxWidth = std::max(maxWidth, static_cast<float>(sName->w));
        totalHeight += static_cast<float>(sName->h) + lineSpacing;
    }
    if (sDesc) {
        maxWidth = std::max(maxWidth, static_cast<float>(sDesc->w));
        totalHeight += static_cast<float>(sDesc->h) + lineSpacing;
    }

    // Draw Key Background
    float bgX = mouseX + 10;
    float bgY = mouseY + 10;
    SDL_FRect bgRect = {bgX, bgY, maxWidth + padding * 2, totalHeight + padding * 2};

    SDL_SetRenderDrawColor(renderer, appContext->GetPreloadColors()->backgroundColor.r,
                           appContext->GetPreloadColors()->backgroundColor.g, appContext->GetPreloadColors()->
                           backgroundColor.b, appContext->GetPreloadColors()->backgroundColor.a);
    SDL_RenderFillRect(renderer, &bgRect);
    SDL_SetRenderDrawColor(renderer, appContext->GetPreloadColors()->borderColor.r,
                           appContext->GetPreloadColors()->borderColor.g, appContext->GetPreloadColors()->
                           borderColor.b, appContext->GetPreloadColors()->borderColor.a);
    SDL_RenderRect(renderer, &bgRect);

    float currentY = bgY + padding;
    float currentX = bgX + padding;

    auto drawSurf = [&](SDL_Surface *s) {
        if (s) {
            SDL_Texture *t = SDL_CreateTextureFromSurface(renderer, s);
            if (t) {
                SDL_FRect dst = {currentX, currentY, static_cast<float>(s->w), static_cast<float>(s->h)};
                SDL_RenderTexture(renderer, t, nullptr, &dst);
                SDL_DestroyTexture(t);
            }
            currentY += static_cast<float>(s->h) + lineSpacing;
            SDL_DestroySurface(s);
        }
    };

    drawSurf(sName);
    drawSurf(sDesc);
}

glimmer::ItemSlotSystem::ItemSlotSystem(WorldContext *worldContext)
    : GameSystem(worldContext) {
    RequireComponent<ItemSlotComponent>();
    RequireComponent<GuiTransform2DComponent>();
}


uint8_t glimmer::ItemSlotSystem::GetRenderOrder() {
    return RENDER_ORDER_DEBUG_HOTBAR;
}

std::string glimmer::ItemSlotSystem::GetName() {
    return "glimmer.ItemSlotSystem";
}
