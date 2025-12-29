//
// Created by Cold-Mint on 2025/12/25.
//

#include "ItemSlotSystem.h"
#include "../../inventory/Item.h"
#include "../../world/WorldContext.h"
#include "../../ecs/component/ItemContainerComonent.h"
#include "../../inventory/DragAndDrop.h"
#include "SDL3_ttf/SDL_ttf.h"
#include <SDL3/SDL.h>
#include <string>
#include <algorithm>

void glimmer::ItemSlotSystem::Render(SDL_Renderer *renderer) {
    const auto entities = worldContext_->GetEntitiesWithComponents<ItemSlotComponent, Transform2DComponent>();
    float mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    const float slotSize = 40.0F * appContext_->GetConfig()->window.uiScale;
    const Item *hoveredItem = nullptr;
    for (auto &entity: entities) {
        const auto slotComp = worldContext_->GetComponent<ItemSlotComponent>(entity->GetID());
        const auto transform = worldContext_->GetComponent<Transform2DComponent>(entity->GetID());

        const Vector2D pos = transform->GetPosition();

        GameEntity *containerEnt = slotComp->GetContainerEntity();
        if (!containerEnt) continue;

        auto containerComp = worldContext_->GetComponent<ItemContainerComponent>(containerEnt->GetID());
        if (!containerComp) continue;

        auto itemContainer = containerComp->GetItemContainer();
        if (!itemContainer) continue;

        int slotIndex = slotComp->GetSlotIndex();
        Item *item = itemContainer->GetItem(slotIndex);
        const SDL_FRect rect = {pos.x, pos.y, slotSize, slotSize};
        float mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        bool isHovered = mouseX >= rect.x && mouseX <= rect.x + rect.w &&
                         mouseY >= rect.y && mouseY <= rect.y + rect.h;
        slotComp->SetHovered(isHovered);
        if (isHovered && item != nullptr) {
            hoveredItem = item;
        }

        DragAndDrop::DrawSlot(appContext_, renderer, pos.x, pos.y, slotSize, item, slotComp->IsSelected(),
                              [&](const DragState &state) {
                                  if (state.sourceType != DragSourceType::INVENTORY || state.sourceContainer ==
                                      nullptr) {
                                      return;
                                  }
                                  auto sourceContainerComp = worldContext_->GetComponent<ItemContainerComponent>(
                                      state.sourceContainer->GetID());
                                  if (sourceContainerComp != nullptr && sourceContainerComp->GetItemContainer() !=
                                      nullptr) {
                                      auto sourceContainer = sourceContainerComp->GetItemContainer();
                                      itemContainer->SwapItem(slotIndex, sourceContainer, state.sourceIndex);
                                  }
                              },
                              [&] {
                                  DragAndDrop::BeginDrag(DragSourceType::INVENTORY, containerEnt, slotIndex,
                                                         item);
                              },
                              [&] {
                                  auto hotBar = worldContext_->GetHotBarComponent();
                                  if (hotBar == nullptr) {
                                      return;
                                  }
                                  hotBar->SetSelectedSlot(slotIndex);
                              }
        );
    }

    if (DragAndDrop::IsDragging()) {
        DragAndDrop::RenderCombined(renderer);
    }

    if (hoveredItem && !DragAndDrop::IsDragging()) {
        RenderTooltip(renderer, hoveredItem);
    }
}

void glimmer::ItemSlotSystem::RenderQuantity(SDL_Renderer *renderer, const SDL_FRect &slotDest, int amount) const {
    const std::string text = std::to_string(amount);
    const SDL_Color color = {255, 255, 255, 255};

    SDL_Surface *surface = TTF_RenderText_Blended(appContext_->GetFont(), text.c_str(), text.length(), color);
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

    // Prepare Text
    const std::string nameInfo = "Name: " + item->GetName();
    const std::string amountInfo = "Amount: " + std::to_string(item->GetAmount());
    const std::string descInfo = item->GetDescription();

    SDL_Color nameColor = {200, 200, 200, 255};
    SDL_Color textColor = {200, 200, 200, 255};

    SDL_Surface *sName = TTF_RenderText_Blended(appContext_->GetFont(), nameInfo.c_str(), nameInfo.length(), nameColor);
    SDL_Surface *sAmount = TTF_RenderText_Blended(appContext_->GetFont(), amountInfo.c_str(), amountInfo.length(),
                                                  textColor);
    // Description might be multiline? Basic implementation for now.
    SDL_Surface *sDesc = TTF_RenderText_Blended(appContext_->GetFont(), descInfo.c_str(), descInfo.length(), textColor);

    float maxWidth = 0;
    float totalHeight = 0;
    const float padding = 5.0f;
    const float lineSpacing = 2.0f;

    if (sName) {
        maxWidth = std::max(maxWidth, static_cast<float>(sName->w));
        totalHeight += static_cast<float>(sName->h) + lineSpacing;
    }
    if (sAmount) {
        maxWidth = std::max(maxWidth, static_cast<float>(sAmount->w));
        totalHeight += static_cast<float>(sAmount->h) + lineSpacing;
    }
    if (sDesc) {
        maxWidth = std::max(maxWidth, static_cast<float>(sDesc->w));
        totalHeight += static_cast<float>(sDesc->h) + lineSpacing;
    }

    // Draw Key Background
    float bgX = mouseX + 10;
    float bgY = mouseY + 10;
    SDL_FRect bgRect = {bgX, bgY, maxWidth + padding * 2, totalHeight + padding * 2};

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
    SDL_RenderFillRect(renderer, &bgRect);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 50);
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
    drawSurf(sAmount);
    drawSurf(sDesc);
}

uint8_t glimmer::ItemSlotSystem::GetRenderOrder() {
    return RENDER_ORDER_DEBUG_HOTBAR; // Or slightly above
}

std::string glimmer::ItemSlotSystem::GetName() {
    return "glimmer.ItemSlotSystem";
}
