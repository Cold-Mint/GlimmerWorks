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
#include "SpiritRendererSystem.h"

#include "core/ecs/component/SpiritRendererComponent.h"
#include "core/world/WorldContext.h"

glimmer::SpiritRendererSystem::SpiritRendererSystem(WorldContext *worldContext) : GameSystem(worldContext) {
    RequireComponent<SpiritRendererComponent>();
    RequireComponent<Transform2DComponent>();
}

void glimmer::SpiritRendererSystem::Render(SDL_Renderer *renderer) {
    if (worldContext_ == nullptr) {
        return;
    }
    const AppContext *appContext = worldContext_->GetAppContext();
    if (appContext == nullptr) {
        return;
    }
    const ResourceLocator *resourceLocator = appContext->GetResourceLocator();
    if (resourceLocator == nullptr) {
        return;
    }
    const CameraComponent *cameraComponent = worldContext_->GetCameraComponent();
    const Transform2DComponent *cameraTransform2D = worldContext_->GetCameraTransform2D();
    if (cameraComponent == nullptr) {
        return;
    }
    if (cameraTransform2D == nullptr) {
        return;
    }
    const std::vector<GameEntity::ID> spiritRendererEntity = worldContext_->GetEntityIDWithComponents<
        Transform2DComponent, SpiritRendererComponent>();
    for (const uint32_t spiritRenderer: spiritRendererEntity) {
        auto spiritRendererComponent = worldContext_->GetComponent<SpiritRendererComponent>(
            spiritRenderer);
        const Transform2DComponent *transform2DComponent = worldContext_->GetComponent<Transform2DComponent>(
            spiritRenderer);
        if (spiritRendererComponent == nullptr || transform2DComponent == nullptr) {
            continue;
        }
        SDL_Texture *sdlTexture = spiritRendererComponent->GetTexture(resourceLocator);
        WorldVector2D worldVector2d = transform2DComponent->GetPosition() + spiritRendererComponent->GetPosition();
        SDL_FRect worldVectorRect = {
            worldVector2d.x,
            worldVector2d.y,
            static_cast<float>(sdlTexture->w),
            static_cast<float>(sdlTexture->h)
        };
        if (cameraComponent->IsRectInViewport(cameraTransform2D->GetPosition(), &worldVectorRect)) {
            CameraVector2D cameraVector2d = cameraComponent->GetViewPortPosition(
                cameraTransform2D->GetPosition(), worldVector2d);
            SDL_FRect dstrect = {
                cameraVector2d.x, cameraVector2d.y, static_cast<float>(sdlTexture->w),
                static_cast<float>(sdlTexture->h)
            };
            SDL_FlipMode flip = SDL_FLIP_NONE;

            if (spiritRendererComponent->IsFlipH()) {
                flip = static_cast<SDL_FlipMode>(flip | SDL_FLIP_HORIZONTAL);
            }
            if (spiritRendererComponent->IsFlipV()) {
                flip = static_cast<SDL_FlipMode>(flip | SDL_FLIP_VERTICAL);
            }
            SDL_RenderTextureRotated(
                renderer,
                sdlTexture,
                nullptr,
                &dstrect,
                0.0f,
                nullptr,
                flip
            );
        }
    }
}

uint8_t glimmer::SpiritRendererSystem::GetRenderOrder() {
    return RENDER_ORDER_SPIRIT_RENDERER;
}

std::string glimmer::SpiritRendererSystem::GetName() {
    return "glimmer.SpiritRendererSystem";
}
