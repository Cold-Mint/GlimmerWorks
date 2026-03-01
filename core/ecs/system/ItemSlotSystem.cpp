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
#include "fmt/format.h"

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
    const std::optional<std::string> descInfo = item->GetDescription();

    AppContext *appContext = worldContext_->GetAppContext();
    LangsResources *langsResources = appContext->GetLangsResources();
    std::vector<SDL_Surface *> surfacesToDraw;
    SDL_Surface *sName = TTF_RenderText_Blended(appContext->GetFont(), nameInfo.c_str(), nameInfo.length(),
                                                appContext->GetPreloadColors()->textColor);
    if (sName) {
        surfacesToDraw.push_back(sName);
    }
    if (descInfo.has_value()) {
        const std::string &description = descInfo.value();
        SDL_Surface *sDesc = TTF_RenderText_Blended_Wrapped(appContext->GetFont(), description.c_str(),
                                                            description.length(),
                                                            appContext->GetPreloadColors()->textColor, 0);
        if (sDesc) {
            surfacesToDraw.push_back(sDesc);
        }
    }

    const VariableConfig &config = item->GetVariableConfig();
    for (auto &definition: config.definition) {
        auto varTextOptional = HumanReadableDisplay(langsResources, &definition);
        if (!varTextOptional.has_value()) {
            continue;
        }
        const std::string &varText = varTextOptional.value();
        const bool positiveAttribute = PositiveAttribute(&definition);
        SDL_Surface *sVar = TTF_RenderText_Blended(
            appContext->GetFont(),
            varText.c_str(),
            varText.length(),
            positiveAttribute
                ? appContext->GetPreloadColors()->game.positiveAttributeColor
                : appContext->GetPreloadColors()->game.negativeAttributeColor
        );
        if (sVar) {
            surfacesToDraw.push_back(sVar);
        }
    }
    const float padding = 5.0f;
    const float lineSpacing = 2.0f;
    float maxWidth = 0.0f;
    float totalHeight = 0.0f;

    for (auto *surf: surfacesToDraw) {
        if (surf) {
            maxWidth = std::max(maxWidth, static_cast<float>(surf->w));
            totalHeight += static_cast<float>(surf->h) + lineSpacing;
        }
    }

    // 移除最后一行多余的行间距
    if (!surfacesToDraw.empty()) {
        totalHeight -= lineSpacing;
    }
    // 绘制背景框
    float bgX = mouseX + 10.0f;
    float bgY = mouseY + 10.0f;
    SDL_FRect bgRect = {
        bgX,
        bgY,
        maxWidth + padding * 2,
        totalHeight + padding * 2
    };
    PreloadColors *preloadColors = appContext->GetPreloadColors();

    // 绘制背景填充
    SDL_SetRenderDrawColor(
        renderer,
        preloadColors->backgroundColor.r,
        preloadColors->backgroundColor.g,
        preloadColors->backgroundColor.b,
        preloadColors->backgroundColor.a
    );
    SDL_RenderFillRect(renderer, &bgRect);

    // 绘制边框
    SDL_SetRenderDrawColor(
        renderer,
        preloadColors->borderColor.r,
        preloadColors->borderColor.g,
        preloadColors->borderColor.b,
        preloadColors->borderColor.a
    );
    SDL_RenderRect(renderer, &bgRect);

    // 绘制所有文本表面
    float currentY = bgY + padding;
    float currentX = bgX + padding;
    for (auto *surf: surfacesToDraw) {
        if (surf) {
            // 创建纹理并绘制
            SDL_Texture *t = SDL_CreateTextureFromSurface(renderer, surf);
            if (t) {
                SDL_FRect dst = {
                    currentX,
                    currentY,
                    static_cast<float>(surf->w),
                    static_cast<float>(surf->h)
                };
                SDL_RenderTexture(renderer, t, nullptr, &dst);
                SDL_DestroyTexture(t); // 用完立即销毁纹理
            }

            // 更新Y坐标
            currentY += static_cast<float>(surf->h) + lineSpacing;

            // 销毁表面
            SDL_DestroySurface(surf);
        }
    }
}

std::optional<std::string> glimmer::ItemSlotSystem::HumanReadableDisplay(const LangsResources *langsResources,
                                                                         const VariableDefinition *variableDefinition) {
    if (variableDefinition->key == "efficiency") {
        const float value = variableDefinition->AsFloat();
        return fmt::format(
            fmt::runtime(langsResources->efficiencyTip),
            (value > 0 ? "+" : "") + std::to_string(variableDefinition->AsFloat() * 100));
    }
    if (variableDefinition->key == "precisionMining" && variableDefinition->AsBool()) {
        return langsResources->precisionMiningTip;
    }
    if (variableDefinition->key == "fumbleChance") {
        const float value = variableDefinition->AsFloat();
        return fmt::format(
            fmt::runtime(langsResources->fumbleTip),
            (value > 0 ? "+" : "") + std::to_string(variableDefinition->AsFloat() * 100));
    }
    return std::nullopt;
}


bool glimmer::ItemSlotSystem::PositiveAttribute(const VariableDefinition *variableDefinition) {
    if (variableDefinition->key == "efficiency") {
        return variableDefinition->AsFloat() > 0;
    }
    if (variableDefinition->key == "precisionMining") {
        return variableDefinition->AsBool();
    }
    if (variableDefinition->key == "fumbleChance") {
        return variableDefinition->AsFloat() <= 0;
    }
    return true;
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
