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
#include "FloatingTextSystem.h"

#include "core/Constants.h"
#include "core/math/CoordinateTransformer.h"
#include "core/ecs/component/FloatingTextComponent.h"
#include "core/ecs/component/Transform2DComponent.h"
#include "core/world/WorldContext.h"

void glimmer::FloatingTextSystem::OnWatchedComponentChanged(GameComponentTypeMessage gameComponentType, uint32_t count)
{
    if (gameComponentType == COMPONENT_CAMERA && cameraComponent_ == nullptr)
    {
        cameraComponent_ = entityShortCut_->GetCameraComponent();
    }
    if (gameComponentType == COMPONENT_TRANSFORM_2D)
    {
        cameraTransform2DComponent_ = entityShortCut_->GetCameraTransform2DComponent();
        transform2DCount_ = count;
    }
    if (gameComponentType == COMPONENT_FLOATING_TEXT)
    {
        floatingTextCount_ = count;
    }
    if (transform2DCount_ > count && floatingTextCount_ > count)
    {
        entities_ = entityManager_->GetEntityIDWithComponents({COMPONENT_TRANSFORM_2D,COMPONENT_FLOATING_TEXT});
    }
}

glimmer::FloatingTextSystem::FloatingTextSystem(WorldContext* worldContext) : GameSystem(worldContext)
{
    WatchComponent(COMPONENT_FLOATING_TEXT);
    WatchComponent(COMPONENT_CAMERA);
    WatchComponent(COMPONENT_TRANSFORM_2D);
}

void glimmer::FloatingTextSystem::Update(float delta)
{
    if (worldContext_ == nullptr)
    {
        return;
    }
    if (cameraComponent_ == nullptr)
    {
        return;
    }
    if (cameraTransform2DComponent_ == nullptr)
    {
        return;
    }
    if (entities_.empty())
    {
        return;
    }
    uint64_t now = SDL_GetTicks();
    for (const GameEntityID floatingTextEntity : entities_)
    {
        auto floatingTextComponent = entityManager_->GetComponent<FloatingTextComponent>(
            floatingTextEntity);
        const Transform2DComponent* transform2DComponent = entityManager_->GetComponent<Transform2DComponent>(
            floatingTextEntity);
        if (floatingTextComponent == nullptr || transform2DComponent == nullptr)
        {
            continue;
        }
        if (!cameraComponent_->
            IsPointInViewport(cameraTransform2DComponent_->GetPosition(), transform2DComponent->GetPosition()))
        {
            continue;
        }
        auto& tween = floatingTextComponent->GetTween();
        tween.step(delta);
        floatingTextComponent->SetAlpha(fabs(tween.peek()));
        if (now > floatingTextComponent->GetExpireTime())
        {
            entityManager_->RemoveEntity(floatingTextEntity);
        }
    }
}

void glimmer::FloatingTextSystem::Render(SDL_Renderer* renderer)
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
    if (cameraComponent_ == nullptr)
    {
        return;
    }
    if (cameraTransform2DComponent_ == nullptr)
    {
        return;
    }
    for (const GameEntityID floatingText : entities_)
    {
        auto floatingTextComponent = entityManager_->GetComponent<FloatingTextComponent>(
            floatingText);
        const Transform2DComponent* transform2DComponent = entityManager_->GetComponent<Transform2DComponent>(
            floatingText);
        if (floatingTextComponent == nullptr || transform2DComponent == nullptr)
        {
            continue;
        }
        if (!cameraComponent_->
            IsPointInViewport(cameraTransform2DComponent_->GetPosition(), transform2DComponent->GetPosition()))
        {
            continue;
        }
        ScreenVector2D camera2D = CoordinateTransformer::WorldToScreen(cameraTransform2DComponent_->GetPosition(),
                                                                  transform2DComponent->GetPosition(), cameraComponent_->GetSize(), cameraComponent_->GetZoom());
        std::string& text = floatingTextComponent->GetText();
        if (text.empty())
        {
            continue;
        }
        if (floatingTextComponent->GetAlpha() <= 0.01F)
        {
            continue;
        }
        SDL_Surface* surface = TTF_RenderText_Blended_Wrapped(appContext->GetFont(), text.c_str(), text.length(),
                                                              appContext->GetPreloadColors()->textColor.ToSDLColor(),
                                                              0);
        if (surface == nullptr)
        {
            continue;
        }
        if (SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface))
        {
            auto textW = static_cast<float>(surface->w);
            auto textH = static_cast<float>(surface->h);
            SDL_FRect dst = {
                camera2D.x,
                camera2D.y,
                textW,
                textH
            };

            SDL_SetTextureAlphaMod(texture, static_cast<Uint8>(floatingTextComponent->GetAlpha() * 255));
            SDL_RenderTexture(renderer, texture, nullptr, &dst);
            SDL_DestroyTexture(texture);
        }
        SDL_DestroySurface(surface);
    }
}

uint8_t glimmer::FloatingTextSystem::GetRenderOrder()
{
    return RENDER_ORDER_FLOATING_TEXT;
}

glimmer::GameSystemType glimmer::FloatingTextSystem::GetGameSystemType() const
{
    return GameSystemType::FloatingTextSystem;
}