/*
 * Copyright (C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * 版权(C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * 本程序是自由软件：你可以遵照自由软件基金会出版的GNU Affero通用公共许可证条款来重新分发和修改它
 * 该许可证的第3版，或者（由你选择）任何后续版本。
 *
 * 本程序的发布目的是希望它能有用，但没有任何担保；甚至没有适销性或特定用途适用性的默示担保。
 * 有关详细细节，请参阅GNU Affero通用公共许可证。
 *
 * 你应该已经收到一份GNU Affero通用公共许可证的副本。如果没有，请查阅<https://www.gnu.org/licenses/>。
 */
#include "ItemSlotSystem.h"
#include "../../inventory/Item.h"
#include "../../world/WorldContext.h"
#include "SDL3_ttf/SDL_ttf.h"
#include <SDL3/SDL.h>
#include <string>
#include <algorithm>
#include "fmt/format.h"

#include "core/ecs/component/GuiTransform2DComponent.h"

void glimmer::ItemSlotSystem::Render(SDL_Renderer* renderer)
{
    if (worldContext_ == nullptr)
    {
        return;
    }
    const AppContext* appContext = worldContext_->GetAppContext();
    if (appContext == nullptr)
    {
        return;
    }
    const auto entities = worldContext_->GetEntityIDWithComponents<ItemSlotComponent, GuiTransform2DComponent>();
    float mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    const Item* hoveredItem = nullptr;
    for (auto& entity : entities)
    {
        const auto slotComp = worldContext_->GetComponent<ItemSlotComponent>(entity);
        const auto transform = worldContext_->GetComponent<GuiTransform2DComponent>(entity);

        const Vector2D pos = transform->GetPosition();
        const Vector2D size = transform->GetSize();

        const Item* item = slotComp->GetItem();
        const SDL_FRect rect = {pos.x, pos.y, size.x, size.y};
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        SDL_RenderRect(renderer, &rect);
        bool isHovered = mouseX >= rect.x && mouseX <= rect.x + rect.w &&
            mouseY >= rect.y && mouseY <= rect.y + rect.h;
        slotComp->SetHovered(isHovered);
        if (slotComp->IsSelected())
        {
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
            SDL_FRect selRect = {rect.x - 2, rect.y - 2, size.x + 4, size.y + 4};
            SDL_RenderRect(renderer, &selRect);
        }
        if (item == nullptr)
        {
            continue;
        }
        auto texture = item->GetIcon();
        if (texture != nullptr)
        {
            SDL_RenderTexture(renderer, texture, nullptr, &rect);
        }
        if (item->GetAmount() > 1)
        {
            std::string text = std::to_string(static_cast<int>(item->GetAmount()));
            SDL_Color color = {255, 255, 255, 255};
            SDL_Surface* surface =
                TTF_RenderText_Blended(appContext->GetFont(), text.c_str(), text.length(), color);
            if (surface)
            {
                SDL_Texture* t = SDL_CreateTextureFromSurface(renderer, surface);
                if (t)
                {
                    SDL_FRect dst = {
                        rect.x + size.x - surface->w - 2, rect.y + size.y - surface->h - 2,
                        static_cast<float>(surface->w),
                        static_cast<float>(surface->h)
                    };
                    SDL_RenderTexture(renderer, t, nullptr, &dst);
                    SDL_DestroyTexture(t);
                }
                SDL_DestroySurface(surface);
            }
        }

        if (isHovered)
        {
            hoveredItem = item;
        }
    }
    if (hoveredItem)
    {
        RenderTooltip(renderer, hoveredItem);
    }
    AppContext::RestoreColorRenderer(renderer);
}

bool glimmer::ItemSlotSystem::HandleEvent(const SDL_Event& event)
{
    if (worldContext_ == nullptr)
    {
        return false;
    }
    if (worldContext_->IsDragMode())
    {
        return false;
    }
    if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT)
    {
        const auto entities = worldContext_->GetEntityIDWithComponents<ItemSlotComponent, GuiTransform2DComponent>();
        if (entities.empty())
        {
            return false;
        }
        float mouseX = event.motion.x;
        float mouseY = event.motion.y;
        ItemSlotComponent* previousItemSlotComponent = nullptr;
        ItemSlotComponent* currentlyItemSlotComponent = nullptr;
        for (uint32_t entity : entities)
        {
            auto* guiTransform2dComponent = worldContext_->GetComponent<
                GuiTransform2DComponent>(entity);
            if (guiTransform2dComponent == nullptr)
            {
                continue;
            }
            auto* itemSlotComponent = worldContext_->GetComponent<ItemSlotComponent>(entity);
            if (itemSlotComponent == nullptr)
            {
                continue;
            }
            const CameraVector2D position = guiTransform2dComponent->GetPosition();
            const CameraVector2D size = guiTransform2dComponent->GetSize();
            const SDL_FRect border = {position.x, position.y, size.x, size.y};
            if (itemSlotComponent->IsSelected())
            {
                previousItemSlotComponent = itemSlotComponent;
            }
            if (itemSlotComponent->AllowSelected())
            {
                bool selected = mouseX >= border.x && mouseX <= border.x + border.w &&
                    mouseY >= border.y && mouseY <= border.y + border.h;
                if (selected)
                {
                    currentlyItemSlotComponent = itemSlotComponent;
                }
            }
        }
        if (currentlyItemSlotComponent != nullptr && previousItemSlotComponent != nullptr && previousItemSlotComponent
            != currentlyItemSlotComponent)
        {
            currentlyItemSlotComponent->SetSelected(true);
            previousItemSlotComponent->SetSelected(false);
        }
    }

    return false;
}

void glimmer::ItemSlotSystem::RenderQuantity(SDL_Renderer* renderer, const SDL_FRect& slotDest, int amount) const
{
    const std::string text = std::to_string(amount);
    AppContext* appContext = worldContext_->GetAppContext();
    SDL_Surface* surface = TTF_RenderText_Blended(appContext->GetFont(), text.c_str(), text.length(),
                                                  appContext->GetPreloadColors()->textColor.ToSDLColor());
    if (surface)
    {
        if (SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface))
        {
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

void glimmer::ItemSlotSystem::RenderTooltip(SDL_Renderer* renderer, const Item* item) const
{
    if (!item) return;

    float mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    const std::string& nameInfo = item->GetName();
    const std::optional<std::string>& descInfo = item->GetDescription();

    const AppContext* appContext = worldContext_->GetAppContext();
    const LangsResources* langsResources = appContext->GetLangsResources();
    std::vector<SDL_Surface*> surfacesToDraw;
    SDL_Surface* sName = TTF_RenderText_Blended(appContext->GetFont(), nameInfo.c_str(), nameInfo.length(),
                                                appContext->GetPreloadColors()->textColor.ToSDLColor());
    if (sName != nullptr)
    {
        surfacesToDraw.push_back(sName);
    }
    if (descInfo.has_value())
    {
        const std::string& description = descInfo.value();
        SDL_Surface* sDesc = TTF_RenderText_Blended_Wrapped(appContext->GetFont(), description.c_str(),
                                                            description.length(),
                                                            appContext->GetPreloadColors()->textColor.ToSDLColor(), 0);
        if (sDesc != nullptr)
        {
            surfacesToDraw.push_back(sDesc);
        }
    }
    if (!item->IsUnbreakable())
    {
        uint32_t totalDur = std::max(item->GetMaxDurability(), 1U);
        uint32_t usedDur = item->GetUsedDurability();
        uint32_t remainDur = usedDur >= totalDur ? 0U : totalDur - usedDur;

        std::string durabilityInfoTip = fmt::format(
            fmt::runtime(langsResources->durabilityInfo),
            std::round(static_cast<float>(remainDur) / static_cast<float>(totalDur) * 100.0f)
        );
        SDL_Surface* durabilityInfo = TTF_RenderText_Blended(appContext->GetFont(), durabilityInfoTip.c_str(),
                                                             durabilityInfoTip.length(),
                                                             appContext->GetPreloadColors()->textColor.ToSDLColor());
        if (durabilityInfo != nullptr)
        {
            surfacesToDraw.push_back(durabilityInfo);
        }
    }

    const AbilityConfig* abilityConfig = item->GetAbilityConfig();
    if (abilityConfig != nullptr)
    {
        if (abilityConfig->mineAbleLayer & Ground)
        {
            SDL_Surface* sVar = TTF_RenderText_Blended(
                appContext->GetFont(),
                langsResources->canMineBlockTip.c_str(),
                langsResources->canMineBlockTip.length(),
                appContext->GetPreloadColors()->game.positiveAttributeColor.ToSDLColor()
            );
            if (sVar)
            {
                surfacesToDraw.push_back(sVar);
            }
        }
        if (abilityConfig->mineAbleLayer & BackGround)
        {
            SDL_Surface* sVar = TTF_RenderText_Blended(
                appContext->GetFont(),
                langsResources->canMineWallTip.c_str(),
                langsResources->canMineWallTip.length(),
                appContext->GetPreloadColors()->game.positiveAttributeColor.ToSDLColor()
            );
            if (sVar)
            {
                surfacesToDraw.push_back(sVar);
            }
        }

        if (abilityConfig->enablePrecisionMining)
        {
            SDL_Surface* sVar = TTF_RenderText_Blended(
                appContext->GetFont(),
                langsResources->precisionMiningTip.c_str(),
                langsResources->precisionMiningTip.length(),
                appContext->GetPreloadColors()->game.positiveAttributeColor.ToSDLColor()
            );
            if (sVar)
            {
                surfacesToDraw.push_back(sVar);
            }
        }

        float miningEfficiency = abilityConfig->miningEfficiency;
        if (miningEfficiency != 0.0F)
        {
            std::string tip = fmt::format(
                fmt::runtime(langsResources->efficiencyTip),
                fmt::format("{0:+.0f}", miningEfficiency * 100)
            );
            SDL_Surface* sVar = TTF_RenderText_Blended(
                appContext->GetFont(),
                tip.c_str(),
                tip.length(),
                miningEfficiency > 0
                    ? appContext->GetPreloadColors()->game.positiveAttributeColor.ToSDLColor()
                    : appContext->GetPreloadColors()->game.negativeAttributeColor.ToSDLColor()
            );
            if (sVar)
            {
                surfacesToDraw.push_back(sVar);
            }
        }

        float fumbleProbability = abilityConfig->fumbleProbability;
        if (fumbleProbability != 0.0F)
        {
            std::string tip = fmt::format(
                fmt::runtime(langsResources->fumbleTip),
                (fumbleProbability > 0 ? "+" : "") + fmt::format("{0:.0f}", fumbleProbability * 100));
            SDL_Surface* sVar = TTF_RenderText_Blended(
                appContext->GetFont(),
                tip.c_str(),
                tip.length(),
                fumbleProbability > 0
                    ? appContext->GetPreloadColors()->game.negativeAttributeColor.ToSDLColor()
                    : appContext->GetPreloadColors()->game.positiveAttributeColor.ToSDLColor()
            );
            if (sVar)
            {
                surfacesToDraw.push_back(sVar);
            }
        }

        int chainMiningRadius = abilityConfig->chainMiningRadius;
        if (chainMiningRadius != 0)
        {
            std::string tip = fmt::format(fmt::runtime(langsResources->chainMiningTip),
                                          (chainMiningRadius > 0 ? "+" : "") + std::to_string(chainMiningRadius));
            SDL_Surface* sVar = TTF_RenderText_Blended(
                appContext->GetFont(),
                tip.c_str(),
                tip.length(),
                chainMiningRadius > 0
                    ? appContext->GetPreloadColors()->game.positiveAttributeColor.ToSDLColor()
                    : appContext->GetPreloadColors()->game.negativeAttributeColor.ToSDLColor()
            );
            if (sVar)
            {
                surfacesToDraw.push_back(sVar);
            }
        }
    }
    const float padding = 5.0f;
    const float lineSpacing = 2.0f;
    float maxWidth = 0.0f;
    float totalHeight = 0.0f;

    for (auto* surf : surfacesToDraw)
    {
        if (surf)
        {
            maxWidth = std::max(maxWidth, static_cast<float>(surf->w));
            totalHeight += static_cast<float>(surf->h) + lineSpacing;
        }
    }

    // 移除最后一行多余的行间距
    if (!surfacesToDraw.empty())
    {
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
    PreloadColors* preloadColors = appContext->GetPreloadColors();

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
    for (auto* surf : surfacesToDraw)
    {
        if (surf)
        {
            // 创建纹理并绘制
            SDL_Texture* t = SDL_CreateTextureFromSurface(renderer, surf);
            if (t)
            {
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

glimmer::ItemSlotSystem::ItemSlotSystem(WorldContext* worldContext)
    : GameSystem(worldContext)
{
    RequireComponent<ItemSlotComponent>();
    RequireComponent<GuiTransform2DComponent>();
}


uint8_t glimmer::ItemSlotSystem::GetRenderOrder()
{
    return RENDER_ORDER_HOTBAR;
}

std::string glimmer::ItemSlotSystem::GetName()
{
    return "glimmer.ItemSlotSystem";
}
