//
// Created by Cold-Mint on 2025/10/29.
//

#include "DebugDrawSystem.h"

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
                SDL_Color oldColor = {255, 255, 255, 255};
                SDL_GetRenderDrawColor(renderer, &oldColor.r, &oldColor.g, &oldColor.b, &oldColor.a);
                const auto color = debugDrawComponent->GetColor();
                SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
                SDL_FRect renderQuad;
                renderQuad.x = worldPositionComponent->GetPosition().x;
                renderQuad.y = worldPositionComponent->GetPosition().y;
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
