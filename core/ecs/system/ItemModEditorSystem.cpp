//
// Created by coldmint on 2025/12/25.
//

#include "ItemModEditorSystem.h"
#include "../../world/WorldContext.h"
#include "../component/HotBarComonent.h"
#include "../component/ItemContainerComonent.h"
#include <string>
#include "../../inventory/AbilityItem.h"

bool glimmer::ItemModEditorSystem::HandleEvent(const SDL_Event &event) {
    auto hotBar = worldContext_->GetHotBarEntity();
    if (WorldContext::IsEmptyEntityId(hotBar)) {
        return false;
    }
    auto hotBarComp = worldContext_->GetComponent<HotBarComponent>(hotBar);
    const auto player = worldContext_->GetPlayerEntity();
    if (WorldContext::IsEmptyEntityId(player)) {
        return false;
    }

    auto containerComp = worldContext_->GetComponent<ItemContainerComponent>(player);
    if (containerComp == nullptr) {
        return false;
    }
    auto itemContainer = containerComp->GetItemContainer();
    if (itemContainer == nullptr) {
        return false;
    }
    const auto selectedSlot = hotBarComp->GetSelectedSlot();
    Item *item = itemContainer->GetItem(selectedSlot);
    if (auto composable = dynamic_cast<ComposableItem *>(item)) {
        editingItem_ = composable;
    } else {
        editingItem_ = nullptr;
        isVisible_ = false;
        return false;
    }
    if (!event.key.repeat && event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_E) {
        isVisible_ = !isVisible_;
        return true;
    }
    return false;
}

void glimmer::ItemModEditorSystem::Render(SDL_Renderer *renderer) {
    // if (!isVisible_) {
    //     return;
    // }
    // if (!editingItem_) {
    //     return;
    // }
    // int w, h;
    // SDL_GetWindowSize(appContext_->GetWindow(), &w, &h);
    //
    // const SDL_FRect bgRect = {
    //     static_cast<float>(w) * 0.2F, static_cast<float>(h) * 0.2F, static_cast<float>(w) * 0.6F,
    //     static_cast<float>(h) * 0.6F
    // };
    // SDL_SetRenderDrawColor(renderer, 50, 50, 50, 240);
    // SDL_RenderFillRect(renderer, &bgRect);
    //
    // SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    // SDL_RenderRect(renderer, &bgRect);
    // std::string title = editingItem_->GetName();
    // SDL_Color color = {255, 255, 255, 255};
    // SDL_Surface *sTitle = TTF_RenderText_Blended(appContext_->GetFont(), title.c_str(), title.length(), color);
    // if (sTitle) {
    //     SDL_Texture *tTitle = SDL_CreateTextureFromSurface(renderer, sTitle);
    //     if (tTitle) {
    //         SDL_FRect dst = {
    //             bgRect.x + 10, bgRect.y + 10, static_cast<float>(sTitle->w), static_cast<float>(sTitle->h)
    //         };
    //         SDL_RenderTexture(renderer, tTitle, nullptr, &dst);
    //         SDL_DestroyTexture(tTitle);
    //     }
    //     SDL_DestroySurface(sTitle);
    // }
    // const auto maxSlotSize = editingItem_->GetMaxSlotSize();
    // const auto &abilityList = editingItem_->GetAbilityList();
    // float startY = bgRect.y + 50;
    //
    // DragAndDrop *dragAndDrop = appContext_->GetDragAndDrop();
    // for (int i = 0; i < maxSlotSize; i++) {
    //     const AbilityItem *ability = i < abilityList.size() ? abilityList[i] : nullptr;
    //
    //
    //     float slotSize = 40.0F; // Assuming standard size
    //     float x = bgRect.x + 20;
    //     float y = startY;
    //
    //     // We use DrawSlot to handle the background and input events.
    //     dragAndDrop->DrawSlot(appContext_, renderer, x, y, slotSize, nullptr, false,
    //                           [&, i](const DragState &state) {
    //                               // On Drop
    //                               if (state.sourceType == DragSourceType::INVENTORY && state.
    //                                   dragedItem) {
    //                                   LogCat::i("Dropped item in editor. ID: ",
    //                                             state.dragedItem->GetId());
    //                                   auto abilityItem = dynamic_cast<AbilityItem *>(state.
    //                                       dragedItem);
    //                                   if (abilityItem == nullptr) {
    //                                       LogCat::w("Dropped item is not an AbilityItem.");
    //                                       return;
    //                                   }
    //                                   GameEntity *sourceContainer = state.sourceContainer;
    //                                   if (sourceContainer == nullptr) {
    //                                       LogCat::w("Source container is null.");
    //                                       return;
    //                                   }
    //                                   auto *itemContainerComponent = worldContext_->GetComponent<
    //                                       ItemContainerComponent>(sourceContainer->GetID());
    //                                   if (itemContainerComponent == nullptr) {
    //                                       LogCat::w("Item container component is null.");
    //                                       return;
    //                                   }
    //                                   editingItem_->SwapItem(
    //                                       i, itemContainerComponent->GetItemContainer(),
    //                                       state.sourceIndex);
    //                               }
    //                           },
    //                           nullptr, // Cannot drag out yet
    //                           nullptr // No click action
    //     );
    //
    //     if (ability != nullptr) {
    //         std::string modName = ability->GetName();
    //         SDL_Color color = {255, 255, 255, 255};
    //         SDL_Surface *sMod =
    //                 TTF_RenderText_Blended(appContext_->GetFont(), modName.c_str(), modName.length(), color);
    //         if (sMod) {
    //             SDL_Texture *tMod = SDL_CreateTextureFromSurface(renderer, sMod);
    //             if (tMod) {
    //                 SDL_FRect dst = {
    //                     x + slotSize + 5, y + 5, static_cast<float>(sMod->w), static_cast<float>(sMod->h)
    //                 };
    //                 SDL_RenderTexture(renderer, tMod, nullptr, &dst);
    //                 SDL_DestroyTexture(tMod);
    //             }
    //             SDL_DestroySurface(sMod);
    //         }
    //     }
    //
    //     startY += slotSize + 5;
    // }
    // if (dragAndDrop->IsDragging()) {
    //     dragAndDrop->RenderCombined(renderer);
    // }
    // AppContext::RestoreColorRenderer(renderer);
}

uint8_t glimmer::ItemModEditorSystem::GetRenderOrder() {
    return RENDER_ORDER_ITEM_MOD_EDITOR;
}

std::string glimmer::ItemModEditorSystem::GetName() {
    return "glimmer.ItemModEditorSystem";
}
