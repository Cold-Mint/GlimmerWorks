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
#include "DebugDrawSystem.h"

#include "../../Constants.h"
#include "../component/CameraComponent.h"
#include "../component/DebugDrawComponent.h"
#include "core/world/WorldContext.h"
#include "core/math/Vector2D.h"


glimmer::DebugDrawSystem::DebugDrawSystem(WorldContext* worldContext) : GameSystem(worldContext)
{
    RequireComponent<DebugDrawComponent>();
    RequireComponent<Transform2DComponent>();
}

void glimmer::DebugDrawSystem::Render(SDL_Renderer* renderer)
{
    const auto* cameraComponent = worldContext_->GetCameraComponent();
    const auto* cameraPos = worldContext_->GetCameraTransform2D();
    if (cameraComponent == nullptr)
    {
        return;
    }
    if (cameraPos == nullptr)
    {
        return;
    }
    auto gameEntities = worldContext_->GetEntityIDWithComponents<DebugDrawComponent, Transform2DComponent>();
    for (auto entity : gameEntities)
    {
        auto debugDrawComponent = worldContext_->GetComponent<DebugDrawComponent>(entity);
        if (debugDrawComponent != nullptr)
        {
            auto worldPositionComponent = worldContext_->GetComponent<Transform2DComponent>(entity);
            if (worldPositionComponent != nullptr)
            {
                auto cameraVector2d = cameraComponent->WorldToScreen(
                    cameraPos->GetPosition(), worldPositionComponent->GetPosition());
                if (!cameraComponent->
                    IsPointInViewport(cameraPos->GetPosition(), worldPositionComponent->GetPosition()))
                {
                    continue;
                }
                SDL_Color oldColor = {255, 255, 255, 255};
                SDL_GetRenderDrawColor(renderer, &oldColor.r, &oldColor.g, &oldColor.b, &oldColor.a);
                const auto color = debugDrawComponent->GetColor();
                SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
                SDL_FRect renderQuad;
                WorldVector2D worldVector2D = debugDrawComponent->GetSize();
                float zoom = cameraComponent->GetZoom();
                float w = worldVector2D.x * zoom;
                float h = worldVector2D.y * zoom;
                renderQuad.w = w;
                renderQuad.h = h;
                renderQuad.x = cameraVector2d.x - w * 0.5F;
                renderQuad.y = cameraVector2d.y - h * 0.5F;
                SDL_RenderFillRect(renderer, &renderQuad);
                // 恢复原先颜色
                SDL_SetRenderDrawColor(renderer, oldColor.r, oldColor.g, oldColor.b, oldColor.a);
            }
        }
    }
}

uint8_t glimmer::DebugDrawSystem::GetRenderOrder()
{
    return RENDER_ORDER_DEBUG_DRAW;
}

std::string glimmer::DebugDrawSystem::GetName()
{
    return "glimmer.DebugDrawSystem";
}
