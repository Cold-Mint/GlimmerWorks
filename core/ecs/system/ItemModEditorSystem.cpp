//
// Created by coldmint on 2025/12/25.
//

#include "ItemModEditorSystem.h"
#include "../../world/WorldContext.h"
#include "../component/HotBarComonent.h"
#include "../component/ItemContainerComonent.h"
#include "SDL3_ttf/SDL_ttf.h"
#include <string>

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
    const auto actualSize = editingItem_->GetModules().size();
    const auto &mods = editingItem_->GetModules();
    float startY = bgRect.y + 50;

    for (int i = 0; i < maxSlotSize; i++) {
        if (i >= actualSize) {
            continue;
        }
        auto mod = mods[i].get();
        if (mod == nullptr) {
            continue;
        }
        std::string modName = mod->GetName();
        SDL_FRect modRect = {bgRect.x + 20, startY, bgRect.w - 40, 30};
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        SDL_RenderFillRect(renderer, &modRect);
        SDL_Surface *sMod = TTF_RenderText_Blended(appContext_->GetFont(), modName.c_str(), modName.length(), color);
        if (sMod) {
            SDL_Texture *tMod = SDL_CreateTextureFromSurface(renderer, sMod);
            if (tMod) {
                SDL_FRect dst = {
                    modRect.x + 5, modRect.y + 5, static_cast<float>(sMod->w), static_cast<float>(sMod->h)
                };
                SDL_RenderTexture(renderer, tMod, nullptr, &dst);
                SDL_DestroyTexture(tMod);
            }
            SDL_DestroySurface(sMod);
        }
        startY += 40;
    }
}

uint8_t glimmer::ItemModEditorSystem::GetRenderOrder() {
    return RENDER_ORDER_ITEM_MOD_EDITOR;
}

std::string glimmer::ItemModEditorSystem::GetName() {
    return "glimmer.ItemModEditorSystem";
}
