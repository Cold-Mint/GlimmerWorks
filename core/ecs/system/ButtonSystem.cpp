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
#include "ButtonSystem.h"

#include "core/math/CoordinateTransformer.h"
#include "core/world/WorldContext.h"

glimmer::ButtonSystem::ButtonSystem(WorldContext* worldContext)
    : GameSystem(worldContext)
{
    WatchComponent(COMPONENT_BUTTON);
    const WorldContext* worldContextPtr = GetWorldContext();
    const AppContext* appContext = worldContextPtr->GetAppContext();
    if (appContext == nullptr)
    {
        return;
    }
    const ResourceLocator* resourceLocator = appContext->GetResourceLocator();

    ResourceRef buttonResourceRef;
    buttonResourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
    buttonResourceRef.SetResourceType(RESOURCE_TEXTURE);

    buttonResourceRef.SetResourceKey("gui/button");
    buttonTextureResult_ = resourceLocator->FindTexture(&buttonResourceRef);

    buttonResourceRef.SetResourceKey("gui/button_hovered");
    buttonHoveredTextureResult_ = resourceLocator->FindTexture(&buttonResourceRef);

    buttonResourceRef.SetResourceKey("gui/button_pressed");
    buttonPressedTextureResult_ = resourceLocator->FindTexture(&buttonResourceRef);

    buttonResourceRef.SetResourceKey("gui/button_disable");
    buttonDisableTextureResult_ = resourceLocator->FindTexture(&buttonResourceRef);

    Init();
}

void glimmer::ButtonSystem::OnWatchedComponentChanged(GameComponentTypeMessage gameComponentType, uint32_t count)
{
    EntityManager* entityManager = GetEntityManager();
    if (gameComponentType == COMPONENT_BUTTON)
    {
        buttonComponents_.clear();
        const std::vector<GameEntityID> buttonEntityVector = entityManager->GetEntityIDWithComponents({
            COMPONENT_BUTTON
        });
        if (buttonEntityVector.empty())
        {
            return;
        }
        for (const GameEntityID buttonEntity : buttonEntityVector)
        {
            auto buttonComponent = entityManager->GetComponent<ButtonComponent>(buttonEntity);
            if (buttonComponent == nullptr)
            {
                continue;
            }
            buttonComponents_.emplace_back(buttonComponent);
        }
    }
}

void glimmer::ButtonSystem::OnConfigChanged(const Config* config)
{
    uiScale_ = config->window.uiScale;
}

void glimmer::ButtonSystem::Update(float delta)
{
    hoveredButton_ = nullptr;
    float mouseX = 0;
    float mouseY = 0;
    SDL_GetMouseState(&mouseX, &mouseY);
    for (auto buttonComponent : buttonComponents_)
    {
        if (!buttonComponent->IsVisible())
        {
            continue;
        }
        const ScreenVector2D positionScreenVector2D = CoordinateTransformer::DesignToScreen(
            buttonComponent->GetPosition(), uiScale_);
        const ScreenVector2D sizeScreenVector2D = CoordinateTransformer::DesignToScreen(
            buttonComponent->GetSize(), uiScale_);
        bool isInside = mouseX >= positionScreenVector2D.x && mouseX < positionScreenVector2D.x + sizeScreenVector2D.x
            && mouseY >= positionScreenVector2D.y && mouseY < positionScreenVector2D.y + sizeScreenVector2D.y;

        if (isInside)
        {
            hoveredButton_ = buttonComponent;
            break;
        }
    }
}

uint8_t glimmer::ButtonSystem::GetRenderOrder()
{
    return RENDER_ORDER_BUTTON;
}

void glimmer::ButtonSystem::RenderButtonBackground(SDL_Renderer* renderer, ButtonComponent* buttonComponent,
                                                   const SDL_FRect& rect) const
{
    const TextureResourceResult* backgroundTextureResult = buttonTextureResult_.get();
    if (hoveredButton_ == buttonComponent)
    {
        if (hoveredButtonPressed)
        {
            backgroundTextureResult = buttonPressedTextureResult_.get();
        }
        else
        {
            backgroundTextureResult = buttonHoveredTextureResult_.get();
        }
    }

    if (backgroundTextureResult != nullptr)
    {
        if (SDL_Texture* texture = backgroundTextureResult->GetResource(); texture != nullptr)
        {
            const ResourcePack* resourcePack = backgroundTextureResult->GetResourcePack();
            if (resourcePack != nullptr)
            {
                const ResourcePackConfig& packConfig = resourcePack->GetResourcePackConfig();
                if (packConfig.buttonNineSlice.enableTiled)
                {
                    SDL_RenderTexture9GridTiled(renderer, texture, nullptr, packConfig.buttonNineSlice.leftBorderPx,
                                                packConfig.buttonNineSlice.rightBorderPx,
                                                packConfig.buttonNineSlice.topBorderPx,
                                                packConfig.buttonNineSlice.bottomBorderPx,
                                                packConfig.buttonNineSlice.scale, &rect,
                                                packConfig.buttonNineSlice.tileScale);
                }
                else
                {
                    SDL_RenderTexture9Grid(renderer, texture, nullptr, packConfig.buttonNineSlice.leftBorderPx,
                                           packConfig.buttonNineSlice.rightBorderPx,
                                           packConfig.buttonNineSlice.topBorderPx,
                                           packConfig.buttonNineSlice.bottomBorderPx,
                                           packConfig.buttonNineSlice.scale, &rect);
                }
            }
        }
    }
}

void glimmer::ButtonSystem::RenderButtonText(SDL_Renderer* renderer, ButtonComponent* buttonComponent,
                                             const ScreenVector2D& position, const ScreenVector2D& size) const
{
    SDL_Texture* textTexture = buttonComponent->GetButtonTextTexture();
    if (hoveredButton_ == buttonComponent)
    {
        if (hoveredButtonPressed)
        {
            textTexture = buttonComponent->GetButtonPressedTextTexture();
        }
        else
        {
            textTexture = buttonComponent->GetButtonHoveredTextTexture();
        }
    }

    if (textTexture != nullptr)
    {
        const auto scaledTextW = static_cast<float>(textTexture->w);
        const auto scaledTextH = static_cast<float>(textTexture->h);
        const float scaledPadding = buttonComponent->GetPadding() * uiScale_;
        const float textX = position.x + scaledPadding + (size.x - scaledTextW - scaledPadding * 2.0F) * 0.5F;
        const float textY = position.y + scaledPadding + (size.y - scaledTextH - scaledPadding * 2.0F) * 0.5F;

        const SDL_FRect textRect{textX, textY, scaledTextW, scaledTextH};
        SDL_RenderTexture(renderer, textTexture, nullptr, &textRect);
    }
}

void glimmer::ButtonSystem::Render(SDL_Renderer* renderer)
{
    for (const auto buttonComponent : buttonComponents_)
    {
        if (buttonComponent == nullptr)
        {
            continue;
        }
        if (!buttonComponent->IsVisible())
        {
            continue;
        }

        const ScreenVector2D& size = CoordinateTransformer::DesignToScreen(
            buttonComponent->GetSize(), uiScale_);
        const ScreenVector2D& position = CoordinateTransformer::DesignToScreen(
            buttonComponent->GetPosition(), uiScale_);
        const SDL_FRect rect = {position.x, position.y, size.x, size.y};

        RenderButtonBackground(renderer, buttonComponent, rect);
        RenderButtonText(renderer, buttonComponent, position, size);
    }
}

bool glimmer::ButtonSystem::HandleEvent(const SDL_Event& event)
{
    if (event.button.button == SDL_BUTTON_LEFT)
    {
        if (hoveredButton_ == nullptr)
        {
            return false;
        }
        hoveredButtonPressed = event.type == SDL_EVENT_MOUSE_BUTTON_DOWN;
        if (hoveredButtonPressed && event.button.reserved)
        {
            hoveredButton_->InvokeClick();
        }
        return true;
    }
    return false;
}

glimmer::GameSystemType glimmer::ButtonSystem::GetGameSystemType() const
{
    return GameSystemType::ButtonSystem;
}
