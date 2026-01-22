//
// Created by coldmint on 2025/12/28.
//

#include "DragAndDrop.h"

#include "DragSourceType.h"
#include "DragState.h"
#include "Item.h"
#include "../scene/AppContext.h"
#include "SDL3_ttf/SDL_ttf.h"

namespace glimmer {
    void DragAndDrop::BeginDrag(DragSourceType type, const GameEntity::ID container, const int index, Item *item) {
        state_.sourceType = type;
        state_.sourceContainer = container;
        state_.sourceIndex = index;
        state_.dragedItem = item;
    }

    void DragAndDrop::EndDrag() {
        state_ = DragState{};
    }

    const DragState &DragAndDrop::GetState() const {
        return state_;
    }

    bool DragAndDrop::IsDragging() const {
        return state_.sourceType != DragSourceType::NONE;
    }

    void DragAndDrop::DrawSlot(const AppContext *appContext, SDL_Renderer *renderer, const CameraVector2D &position,
                               const CameraVector2D &size,
                               const Item *item, bool isSelected, const std::function<void(const DragState &)> &onDrop,
                               const std::function<void()> &onDragStart, const std::function<void()> &onClick) {
        const SDL_FRect rect = {position.x, position.y, size.x, size.y};
        float mouseX, mouseY;
        auto mouseState = SDL_GetMouseState(&mouseX, &mouseY);
        bool isHovered = mouseX >= rect.x && mouseX <= rect.x + rect.w &&
                         mouseY >= rect.y && mouseY <= rect.y + rect.h;

        bool isMouseDown = (mouseState & SDL_BUTTON_LMASK) != 0;
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        SDL_RenderRect(renderer, &rect);

        // Draw Item
        if (item != nullptr) {
            // If this is the item being dragged, draw it semi-transparent
            if (IsDragging() && state_.dragedItem == item) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 128);
            }

            auto texture = item->GetIcon();
            if (texture) {
                SDL_RenderTexture(renderer, texture.get(), nullptr, &rect);
            }
            // Quantity
            if (item->GetAmount() > 1) {
                std::string text = std::to_string(static_cast<int>(item->GetAmount()));
                SDL_Color color = {255, 255, 255, 255};
                SDL_Surface *surface =
                        TTF_RenderText_Blended(appContext->GetFont(), text.c_str(), text.length(), color);
                if (surface) {
                    SDL_Texture *t = SDL_CreateTextureFromSurface(renderer, surface);
                    if (t) {
                        SDL_FRect dst = {
                            position.x + size.x - surface->w - 2, position.y + size.y - surface->h - 2,
                            static_cast<float>(surface->w),
                            static_cast<float>(surface->h)
                        };
                        SDL_RenderTexture(renderer, t, nullptr, &dst);
                        SDL_DestroyTexture(t);
                    }
                    SDL_DestroySurface(surface);
                }
            }
        }

        if (isSelected) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
            SDL_FRect selRect = {position.x - 2, position.y - 2, size.x + 4, size.y + 4};
            SDL_RenderRect(renderer, &selRect);
        }

        // Logic
        if (isHovered) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 50);
            SDL_RenderFillRect(renderer, &rect);

            if (!isMouseDown) {
                if (IsDragging()) {
                    // Drop
                    if (onDrop) onDrop(state_);
                    EndDrag();
                } else {
                    // Click
                    if (onClick) onClick();
                }
            } else {
                if (!IsDragging() && item && onDragStart) {
                    // Start Drag
                    onDragStart();
                }
            }
        }
    }

    void DragAndDrop::RenderCombined(SDL_Renderer *renderer) const {
        if (!IsDragging() || !state_.dragedItem) return;

        float x, y;
        SDL_GetMouseState(&x, &y);

        // Draw centered on mouse
        float size = 40.0F; // Default size, maybe scale?
        SDL_FRect rect = {x - size / 2, y - size / 2, size, size};

        auto texture = state_.dragedItem->GetIcon();
        if (texture) {
            SDL_RenderTexture(renderer, texture.get(), nullptr, &rect);
        }
    }
}
