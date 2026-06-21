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
#include "ParallaxBackgroundSystem.h"

#include "SDL3/SDL.h"
#include "core/Constants.h"
#include "core/ecs/component/ParallaxBackgroundComponent.h"
#include "core/world/WorldContext.h"

glimmer::ParallaxBackgroundSystem::ParallaxBackgroundSystem(WorldContext* worldContext) : GameSystem(worldContext)
{
    WatchComponent(COMPONENT_PARALLAX_BACKGROUND);
    Init();
}

void glimmer::ParallaxBackgroundSystem::Render(SDL_Renderer* renderer)
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
    const ResourceLocator* resourceLocator = appContext->GetResourceLocator();
    if (resourceLocator == nullptr)
    {
        return;
    }
    ParallaxBackgroundComponent* parallaxBackgroundComponent = entityShortCut_->GetParallaxBackgroundComponent();
    if (parallaxBackgroundComponent == nullptr)
    {
        return;
    }
    SDL_Texture* texture = parallaxBackgroundComponent->GetTexture(resourceLocator);
    if (texture == nullptr)
    {
        return;
    }
    SDL_FRect destRect = {
        0.0F,
        0.0F,
        static_cast<float>(appContext->GetWindowWidth()),
        static_cast<float>(appContext->GetWindowHeight()),
    };
    SDL_RenderTexture(renderer, texture, nullptr, &destRect);
}

glimmer::GameSystemType glimmer::ParallaxBackgroundSystem::GetGameSystemType() const
{
    return GameSystemType::ParallaxBackgroundSystem;
}

uint8_t glimmer::ParallaxBackgroundSystem::GetRenderOrder()
{
    return RENDER_ORDER_PARALLAX_BACKGROUND;
}
