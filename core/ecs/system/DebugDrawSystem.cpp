//
// Created by Cold-Mint on 2025/10/29.
//

#include "DebugDrawSystem.h"

#include "../component/CameraComponent.h"
#include "../component/DebugDrawComponent.h"


void glimmer::DebugDrawSystem::Render(SDL_Renderer* renderer)
{
    auto cameraComponent = worldContext_->GetCameraComponent();
    auto cameraPos = worldContext_->GetCameraPosition();
    if (cameraComponent == nullptr)
    {
        return;
    }
    if (cameraPos == nullptr)
    {
        return;
    }
    auto entitys = worldContext_->GetEntitiesWithComponents<DebugDrawComponent, WorldPositionComponent>();
    for (auto entity : entitys)
    {
        auto debugDrawComponent = worldContext_->GetComponent<DebugDrawComponent>(entity->GetID());
        if (debugDrawComponent != nullptr)
        {
            auto worldPositionComponent = worldContext_->GetComponent<WorldPositionComponent>(entity->GetID());
            if (worldPositionComponent != nullptr)
            {
                auto cameraVector2d = cameraComponent->GetViewPortPosition(
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
                const float zoom = cameraComponent->GetZoom();
                const auto objSize = debugDrawComponent->GetSize();
                const float scaledW = objSize.x * zoom;
                const float scaledH = objSize.y * zoom;
                const auto viewportRect = cameraComponent->GetViewportRect(cameraPos->GetPosition());
                SDL_FPoint screenCenter = {viewportRect.w * 0.5f, viewportRect.h * 0.5f};
                SDL_FPoint objCenter = {cameraVector2d.x, cameraVector2d.y};
                SDL_FPoint centeredOffset = {objCenter.x - screenCenter.x, objCenter.y - screenCenter.y};
                SDL_FPoint scaledOffset = {centeredOffset.x * zoom, centeredOffset.y * zoom};
                SDL_FPoint adjustedCenter = {screenCenter.x + scaledOffset.x, screenCenter.y + scaledOffset.y};
                SDL_FRect renderQuad;
                renderQuad.w = scaledW;
                renderQuad.h = scaledH;
                renderQuad.x = adjustedCenter.x - scaledW * 0.5f;
                renderQuad.y = adjustedCenter.y - scaledH * 0.5f;
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
