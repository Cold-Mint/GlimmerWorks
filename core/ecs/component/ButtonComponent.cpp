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
#include "ButtonComponent.h"

#include <utility>

#include "core/scene/AppContext.h"

const glimmer::DesignVector2D& glimmer::ButtonComponent::GetPosition() const
{
    return position_;
}

void glimmer::ButtonComponent::SetClickCallback(std::function<void()> onClick)
{
    onClick_ = std::move(onClick);
}

void glimmer::ButtonComponent::InvokeClick() const
{
    if (onClick_ == nullptr)
    {
        return;
    }
    onClick_();
}

void glimmer::ButtonComponent::SetText(const AppContext* appContext, const std::string& text)
{
    if (appContext == nullptr)
    {
        return;
    }
    text_ = text;
    ResourcePackManager* resourcePackManager = appContext->GetResourcePackManager();
    if (resourcePackManager == nullptr)
    {
        return;
    }
    const PreloadColors* preloadColors = appContext->GetPreloadColors();
    if (preloadColors == nullptr)
    {
        return;
    }
    buttonTextTexture_ = resourcePackManager->CreateStringTexture(text, &preloadColors->buttonTextColor);
    buttonHoveredTextTexture_ = resourcePackManager->CreateStringTexture(text, &preloadColors->buttonHoveredTextColor);
    buttonPressedTextTexture_ = resourcePackManager->CreateStringTexture(text, &preloadColors->buttonPressedTextColor);
    Config* config = appContext->GetConfig();
    if (config == nullptr)
    {
        return;
    }
    float uiScale = config->window.uiScale;
    size_.x = static_cast<float>(buttonTextTexture_.get()->w) / uiScale + padding_ * 2.0F;
    size_.y = static_cast<float>(buttonTextTexture_.get()->h) / uiScale + padding_ * 2.0F;
}

SDL_Texture* glimmer::ButtonComponent::GetButtonTextTexture() const
{
    if (buttonTextTexture_ == nullptr)
    {
        return nullptr;
    }
    return buttonTextTexture_.get();
}

SDL_Texture* glimmer::ButtonComponent::GetButtonHoveredTextTexture() const
{
    if (buttonHoveredTextTexture_ == nullptr)
    {
        return nullptr;
    }
    return buttonHoveredTextTexture_.get();
}

SDL_Texture* glimmer::ButtonComponent::GetButtonPressedTextTexture() const
{
    if (buttonPressedTextTexture_ == nullptr)
    {
        return nullptr;
    }
    return buttonPressedTextTexture_.get();
}

const std::string& glimmer::ButtonComponent::GetText() const
{
    return text_;
}

const glimmer::DesignVector2D& glimmer::ButtonComponent::GetSize() const
{
    return size_;
}

void glimmer::ButtonComponent::SetPosition(const DesignVector2D& position)
{
    position_ = position;
}

void glimmer::ButtonComponent::SetPadding(DesignDimension padding)
{
    padding_ = padding;
}

DesignDimension glimmer::ButtonComponent::GetPadding() const
{
    return padding_;
}

GameComponentTypeMessage glimmer::ButtonComponent::GetComponentTypeStatic()
{
    return COMPONENT_BUTTON;
}

GameComponentTypeMessage glimmer::ButtonComponent::GetComponentType()
{
    return GetComponentTypeStatic();
}
