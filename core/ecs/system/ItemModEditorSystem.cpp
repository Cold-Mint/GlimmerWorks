//
// Created by coldmint on 2025/12/25.
//

#include "ItemModEditorSystem.h"
#include "../../world/WorldContext.h"
#include "../component/HotBarComonent.h"
#include "../component/ItemContainerComonent.h"
#include "SDL3_ttf/SDL_ttf.h"
#include <string>
#include "../../inventory/DragAndDrop.h"
#include "../../inventory/AbilityItem.h"
#include "../../inventory/ability/DigBlockFunctionMod.h"

void glimmer::ItemModEditorSystem::ToggleEditor() {
    isVisible_ = !isVisible_;
    if (!isVisible_) {
        editingItem_ = nullptr;
        return;
    }

    // Try to get the item in hand
    const auto player = worldContext_->GetPlayerEntity();
    if (player == nullptr) { return; }

    auto hotBar = worldContext_->GetHotBarComponent();
    auto containerComp = worldContext_->GetComponent<ItemContainerComponent>(player->GetID());

    if (hotBar && containerComp) {
        if (auto itemContainer = containerComp->GetItemContainer()) {
            Item *item = itemContainer->GetItem(hotBar->GetSelectedSlot());
            // Check if it is composable
            if (auto composable = dynamic_cast<ComposableItem *>(item)) {
                editingItem_ = composable;
            } else {
                // If not composable, maybe we shouldn't open editor or open empty one?
                // For now, only open if composable
                isVisible_ = false;
            }
        }
    }
}

bool glimmer::ItemModEditorSystem::HandleEvent(const SDL_Event &event) {
    if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_E) {
        ToggleEditor();
        return true;
    }
    return false;
}

bool glimmer::ItemModEditorSystem::ShouldActivate() {
    return true;
}

void glimmer::ItemModEditorSystem::Render(SDL_Renderer *renderer) {
    if (!isVisible_) {
        return;
    }
    if (!editingItem_) {
        return;
    }

    // Draw centralized background
    int w, h;
    SDL_GetWindowSize(appContext_->GetWindow(), &w, &h);

    SDL_FRect bgRect = {
        static_cast<float>(w) * 0.2F, static_cast<float>(h) * 0.2F, static_cast<float>(w) * 0.6F,
        static_cast<float>(h) * 0.6F
    };
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 240);
    SDL_RenderFillRect(renderer, &bgRect);

    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderRect(renderer, &bgRect);

    // Draw Title
    std::string title = editingItem_->GetName();
    SDL_Color color = {255, 255, 255, 255};
    SDL_Surface *sTitle = TTF_RenderText_Blended(appContext_->GetFont(), title.c_str(), title.length(), color);
    if (sTitle) {
        SDL_Texture *tTitle = SDL_CreateTextureFromSurface(renderer, sTitle);
        if (tTitle) {
            SDL_FRect dst = {
                bgRect.x + 10, bgRect.y + 10, static_cast<float>(sTitle->w), static_cast<float>(sTitle->h)
            };
            SDL_RenderTexture(renderer, tTitle, nullptr, &dst);
            SDL_DestroyTexture(tTitle);
        }
        SDL_DestroySurface(sTitle);
    }
    const auto maxSlotSize = editingItem_->GetMaxSlotSize();
    const auto &mods = editingItem_->GetAbilityList();
    float startY = bgRect.y + 50;

    for (int i = 0; i < maxSlotSize; i++) {
        const ItemAbility* mod = (i < mods.size()) ? mods[i].get() : nullptr;

        
        float slotSize = 40.0f; // Assuming standard size
        float x = bgRect.x + 20;
        float y = startY;
        
        // We use DrawSlot to handle the background and input events.
        DragAndDrop::DrawSlot(appContext_, renderer, x, y, slotSize, nullptr, false,
            [&, i](const DragState& state) {
                // On Drop
                if (state.sourceType == DragSourceType::INVENTORY && state.dragedItem) {
                     LogCat::i("Dropped item in editor. ID: ", state.dragedItem->GetId());
                     // Check if it is AbilityItem
                     const auto abilityItem = dynamic_cast<const AbilityItem*>(state.dragedItem);
                     if (abilityItem) {
                         // Add ability to ComposableItem
                         // Hardcoded mapping for now
                         if (abilityItem->GetId().find("dig") != std::string::npos) { // Broader ID check
                             LogCat::i("Adding DigBlockFunctionMod to item.");
                             editingItem_->AddItemAbility(std::make_unique<DigBlockFunctionMod>());
                             
                             // Consume the item from source
                             GameEntity* srcEntity = state.sourceContainer;
                             if (srcEntity) {
                                 auto containerComp = worldContext_->GetComponent<ItemContainerComponent>(srcEntity->GetID());
                                 if (containerComp) {
                                     auto container = containerComp->GetItemContainer();
                                     if (container) {
                                         container->RemoveItemAt(state.sourceIndex, 1);
                                     }
                                 }
                             }
                         } else {
                             // Generic fallback or logging
                             LogCat::w("Unknown ability item dropped: ", abilityItem->GetId(), " Name: ", abilityItem->GetName());
                         }
                     } else {
                         LogCat::w("Dropped item is not an AbilityItem.");
                     }
                }
            },
            nullptr, // Cannot drag out yet
            nullptr  // No click action
        );
        
        // Manually Draw Mod Info if it exists (since DrawSlot only draws Item icons)
        if (mod) {
            std::string modName = mod->GetName();
            SDL_Color color = {255, 255, 255, 255};
            SDL_Surface *sMod = TTF_RenderText_Blended(appContext_->GetFont(), modName.c_str(), modName.length(), color);
            if (sMod) {
                SDL_Texture *tMod = SDL_CreateTextureFromSurface(renderer, sMod);
                if (tMod) {
                    SDL_FRect dst = {
                        x + slotSize + 5, y + 5, static_cast<float>(sMod->w), static_cast<float>(sMod->h)
                    };
                    SDL_RenderTexture(renderer, tMod, nullptr, &dst);
                    SDL_DestroyTexture(tMod);
                }
                SDL_DestroySurface(sMod);
            }
        }
        
        startY += slotSize + 5;
    }
    
    // Ensure we render the dragged item on top if we are dragging inside this system
    if (DragAndDrop::IsDragging()) {
         DragAndDrop::RenderCombined(renderer);
    }
}

uint8_t glimmer::ItemModEditorSystem::GetRenderOrder() {
    return RENDER_ORDER_ITEM_MOD_EDITOR;
}

std::string glimmer::ItemModEditorSystem::GetName() {
    return "glimmer.ItemModEditorSystem";
}
