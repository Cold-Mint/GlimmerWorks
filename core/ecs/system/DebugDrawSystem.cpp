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
                // LogCat::d("Entity id", entity->GetID(), " Camera coordinates ", cameraPos->GetPosition().x, " ",
                //           cameraPos->GetPosition().y, " Object screen coordinates", cameraVector2d.x, " ",
                //           cameraVector2d.y, " Object pos", worldPositionComponent->GetPosition().x, " ",
                //           worldPositionComponent->GetPosition().y);
                //Determine whether the coordinates are included in the screen?
                //判断坐标是否包含在屏幕内？
                if (!cameraComponent->
                    IsPointInViewport(cameraPos->GetPosition(), worldPositionComponent->GetPosition()))
                {
                    // LogCat::d("Entity id Not In Point", entity->GetID());
                    continue;
                }
                SDL_Color oldColor = {255, 255, 255, 255};
                SDL_GetRenderDrawColor(renderer, &oldColor.r, &oldColor.g, &oldColor.b, &oldColor.a);
                const auto color = debugDrawComponent->GetColor();
                SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
                SDL_FRect renderQuad;
                renderQuad.x = cameraVector2d.x;
                renderQuad.y = cameraVector2d.y;
                renderQuad.w = debugDrawComponent->GetSize().x;
                renderQuad.h = debugDrawComponent->GetSize().y;
                SDL_RenderFillRect(renderer, &renderQuad);
                SDL_SetRenderDrawColor(renderer, oldColor.r, oldColor.g, oldColor.b, oldColor.a);
            }
        }
    }
}

std::string glimmer::DebugDrawSystem::GetName()
{
    return "glimmer.DebugDrawSystem";
}
