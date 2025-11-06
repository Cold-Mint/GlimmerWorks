//
// Created by Cold-Mint on 2025/10/29.
//

#include "DebugDrawSystem.h"

#include "../../Constants.h"
#include "../component/CameraComponent.h"
#include "../component/DebugDrawComponent.h"
#include "../../world/WorldContext.h"


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
    auto entitys = worldContext_->GetEntitiesWithComponents<DebugDrawComponent, Transform2DComponent>();
    for (auto entity : entitys)
    {
        auto debugDrawComponent = worldContext_->GetComponent<DebugDrawComponent>(entity->GetID());
        if (debugDrawComponent != nullptr)
        {
            auto worldPositionComponent = worldContext_->GetComponent<Transform2DComponent>(entity->GetID());
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
                SDL_FRect renderQuad;
                float w = debugDrawComponent->GetSize().x * cameraComponent->GetZoom();
                float h = debugDrawComponent->GetSize().y * cameraComponent->GetZoom();
                renderQuad.w = w;
                renderQuad.h = h;
                renderQuad.x = cameraVector2d.x - w * 0.5f;
                renderQuad.y = cameraVector2d.y - h * 0.5f;
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
